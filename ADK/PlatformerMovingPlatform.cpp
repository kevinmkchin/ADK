#include "PlatformerMovingPlatform.h"
#include "PlatformerPlayer.h"
#include "Engine/ADKMath.h"

ADKOBJECT(PlatformerMovingPlatform)
PlatformerMovingPlatform::PlatformerMovingPlatform()
	: direction(45.f)
	, distance_to_travel(50.f)
	, travel_time_in_seconds(3.f)
	, pause_at_turn_in_seconds(1.f)
	, b_going_towards_end(false)
	, travel_timer(0.f)
{
	ADKOBJECT_BEGIN(PlatformerMovingPlatform)
	ADKOBJECT_END()

	texture_path = "Game/movingplatform24x16.png";
	set_frame_size(24, 16);
	set_animation_start_frame(0, 0);
	sprite_sheet.animations[0].anim_duration = sf::seconds(0.2f);
	sprite_sheet.animations[0].num_frames = 4;

	init_collider();
}

void PlatformerMovingPlatform::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 24.f, 16.f);
}

void PlatformerMovingPlatform::update(float deltaTime)
{
	Entity::update(deltaTime);

	sf::Vector2f vel_this_tick = (b_going_towards_end ? 1.f : -1.f) * (velocity_per_sec * deltaTime);

	travel_timer += deltaTime;
	if (travel_timer < travel_time_in_seconds)
	{
		if (entity_to_collide != nullptr && entity_to_collide->is_active())
		{
			if (collider.will_touch_top(entity_to_collide->get_collider(), 0.01f))
			{
				entity_to_collide->move((b_going_towards_end ? 1.f : -1.f) * (velocity_per_sec * deltaTime));
			}
			else if (vel_this_tick.x < 0 && collider.will_touch_left(entity_to_collide->get_collider(), vel_this_tick.x))
			{
				entity_to_collide->move(vel_this_tick.x, 0.f);
			}
			else if (vel_this_tick.x > 0 && collider.will_touch_right(entity_to_collide->get_collider(), vel_this_tick.x))
			{
				entity_to_collide->move(vel_this_tick.x, 0.f);
			}
		}

		move((b_going_towards_end ? 1.f : -1.f) * (velocity_per_sec * deltaTime));
	}
	else if (travel_timer >= travel_time_in_seconds && travel_timer < travel_time_in_seconds + pause_at_turn_in_seconds)
	{
		b_anim_paused = true;
	}
	else if (travel_timer >= travel_time_in_seconds + pause_at_turn_in_seconds)
	{
		b_anim_paused = false;
		travel_timer = 0.f;
		b_going_towards_end = !b_going_towards_end;
	}
}

void PlatformerMovingPlatform::begin_play()
{
	Entity::begin_play();

	sf::Vector2f travel_distance(distance_to_travel, 0.f);
	travel_distance = ADKMath::rotate_vector(travel_distance, direction);
	velocity_per_sec = travel_distance / travel_time_in_seconds;
}
