#include "ADKCamera.h"
#include "ADKMath.h"
#include "Entity.h"

ADKCamera::ADKCamera()
	: render_window_ptr(nullptr)
	, unmodified_center(sf::Vector2f(0.f, 0.f))
	, b_follow_target_x(false)
	, b_follow_target_y(false)
	, entity_target(nullptr)
	, follow_offset(sf::Vector2f(0.f, 0.f))
	, camera_shake_offset(sf::Vector2f(0.f, 0.f))
	, x_intensity_original(0.f)
	, y_intensity_original(0.f)
	, shake_lerp_amount(0.f)
	, shake_duration(0.f)
	, b_shake_at_intensity(false)
	, x_intensity_curr(0.f)
	, y_intensity_curr(0.f)
{

}

ADKCamera::ADKCamera(sf::RenderWindow& window)
	: render_window_ptr(nullptr)
	, b_follow_target_x(false)
	, b_follow_target_y(false)
	, entity_target(nullptr)
	, follow_offset(sf::Vector2f(0.f, 0.f))
	, camera_shake_offset(sf::Vector2f(0.f, 0.f))
{
	set_window(window);
}

void ADKCamera::update_camera(float dt)
{
	if (render_window_ptr == nullptr)
	{
		printf("No window specified for ADKCamera. \n");
		return;
	}

	// Reset center on every tick (reverts modification from camera shake, etc.)
	active_camera_view.setCenter(unmodified_center); 

	// Follow
	if (entity_target != nullptr)
	{
		sf::Vector2f new_center = active_camera_view.getCenter();
		if (b_follow_target_x)
		{
			unmodified_center.x = entity_target->get_position().x;
			new_center.x = entity_target->get_position().x + follow_offset.x;
		}
		if (b_follow_target_y)
		{
			unmodified_center.y = entity_target->get_position().y;
			new_center.y = entity_target->get_position().y + follow_offset.y;
		}
		active_camera_view.setCenter(new_center);
	}

	// Camera shake
	if (shake_duration > 0.f)
	{
		float x_shake;
		float y_shake;
		if (b_shake_at_intensity)
		{
			x_shake = ADKMath::rand_float_range(-x_intensity_curr, x_intensity_curr);
			y_shake = ADKMath::rand_float_range(-y_intensity_curr, y_intensity_curr);
		}
		else
		{
			x_shake = x_intensity_curr * (ADKMath::flip_coin() ? -1.f : 1.f);
			y_shake = y_intensity_curr * (ADKMath::flip_coin() ? -1.f : 1.f);
		}
		camera_shake_offset.x = x_shake;
		camera_shake_offset.y = y_shake;
		x_intensity_curr *= shake_lerp_amount;
		y_intensity_curr *= shake_lerp_amount;

		if (x_intensity_curr < 0.05f)
		{
			x_intensity_curr = 0.f;
		}
		if (y_intensity_curr < 0.05f)
		{
			y_intensity_curr = 0.f;
		}

		shake_duration -= dt;
	}
	else
	{
		camera_shake_offset.x = 0.f;
		camera_shake_offset.y = 0.f;
	}
	sf::Vector2f curr_center = active_camera_view.getCenter();
	curr_center += camera_shake_offset;
	active_camera_view.setCenter(curr_center);

	// Finally apply updated view
	render_window_ptr->setView(active_camera_view);
}

void ADKCamera::shake_camera(float in_x_intensity, float in_y_intensity, float in_lerp_amount, float in_duration, bool b_at_intensity)
{
	if (render_window_ptr == nullptr)
	{
		printf("No window specified for ADKCamera. \n");
		return;
	}

	if (in_lerp_amount < 0.f)
	{
		in_lerp_amount = 0.f;
	}
	if (in_lerp_amount > 1.f)
	{
		in_lerp_amount = 1.f;
	}

	// remember values
	x_intensity_original = in_x_intensity;
	y_intensity_original = in_y_intensity;
	shake_lerp_amount = in_lerp_amount;
	shake_duration = in_duration;
	b_shake_at_intensity = b_at_intensity;
	x_intensity_curr = in_x_intensity;
	y_intensity_curr = in_y_intensity;

	float x_shake;
	float y_shake;
	if (b_shake_at_intensity)
	{
		x_shake = ADKMath::rand_float_range(-x_intensity_curr, x_intensity_curr);
		y_shake = ADKMath::rand_float_range(-y_intensity_curr, y_intensity_curr);
	}
	else
	{
		x_shake = x_intensity_curr * (ADKMath::flip_coin() ? -1.f : 1.f);
		y_shake = y_intensity_curr * (ADKMath::flip_coin() ? -1.f : 1.f);
	}
	camera_shake_offset.x = x_shake;
	camera_shake_offset.y = y_shake;
	x_intensity_curr *= shake_lerp_amount;
	y_intensity_curr *= shake_lerp_amount;
}

void ADKCamera::set_position(float in_x, float in_y)
{
	unmodified_center.x = in_x;
	unmodified_center.y = in_y;

	active_camera_view.setCenter(unmodified_center);
}

void ADKCamera::set_window(sf::RenderWindow& window)
{
	render_window_ptr = &window;
	active_camera_view = window.getView();
	unmodified_center = window.getView().getCenter();
}

void ADKCamera::set_follow_target(Entity* in_entity)
{
	entity_target = in_entity;
	follow_offset.x = ((float)entity_target->sprite_sheet.frame_size.x / 2.f);
	follow_offset.y = ((float)entity_target->sprite_sheet.frame_size.y / 2.f);
}

