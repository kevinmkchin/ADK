#include "PlatformerFallingPlatform.h"
#include "PlatformerPlayer.h"

PlatformerFallingPlatform::PlatformerFallingPlatform()
	: initial_delay_seconds(0.8f)
	, fall_acceleration(40.f)
	, stop_y(800.f)
	, initial_delay_timer(0.f)
	, curr_fall_vel(0.f)
	, b_falling_active(false)
{
	texture_path = "Game/movingplatforms.png";

	init_collider();
}

void PlatformerFallingPlatform::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 16.f, 16.f);
}

void PlatformerFallingPlatform::update(float deltaTime)
{
	Entity::update(deltaTime);

	update_falling_active();

	if (b_falling_active)
	{
		// initial delay
		if (initial_delay_timer < initial_delay_seconds)
		{
			initial_delay_timer += deltaTime;
			return;
		}

		// accelerate
		curr_fall_vel += fall_acceleration * deltaTime;

		// move player
		if (collider.will_touch_top(entity_to_collide->get_collider(), 0.01f))
		{
			entity_to_collide->move(0.f, curr_fall_vel * deltaTime);
		}

		// move block
		move(0.f, curr_fall_vel * deltaTime);
	}
}

void PlatformerFallingPlatform::update_falling_active()
{
	if (get_position().y >= stop_y)
	{
		b_falling_active = false;
	}
	else
	{
		if (collider.will_touch_top(entity_to_collide->get_collider(), 0.01f))
		{
			b_falling_active = true;
		}
	}
}
