#include "PlatformerFallingPlatform.h"
#include "PlatformerOneWayTile.h"
#include "PlatformerPlayer.h"
#include "Engine/ADKCamera.h"
#include "Engine/EntityList.h"

ADKOBJECT(PlatformerFallingPlatform)
PlatformerFallingPlatform::PlatformerFallingPlatform()
	: initial_delay_seconds(0.6f)
	, fall_acceleration(40.f)
	, stop_y(800.f)
	, initial_delay_timer(0.f)
	, curr_fall_vel(0.f)
	, max_fall_vel(260.f)
	, b_falling_active(false)
{
	ADKOBJECT_BEGIN(PlatformerFallingPlatform)
	ADKOBJECT_FIELD(initial_delay_seconds)
	ADKOBJECT_FIELD(fall_acceleration)
	ADKOBJECT_FIELD(stop_y)
	ADKOBJECT_FIELD(max_fall_vel)
	ADKOBJECT_END()

	texture_path = "Game/fallingplatform_a_24x16.png";
	set_frame_size(24, 16);
	set_animation_start_frame(0, 0);
	sprite_sheet.animations[0].anim_duration = sf::seconds(initial_delay_seconds);
	sprite_sheet.animations[0].num_frames = 4;
	sprite_sheet.b_repeat = false;
	b_anim_paused = true;

	set_depth(-1);

	init_collider();
}

void PlatformerFallingPlatform::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 24.f, 16.f);
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
		if (curr_fall_vel > max_fall_vel)
		{
			curr_fall_vel = max_fall_vel;
		}

		// collisions and movement
		bool b_y_collided = false;
		EntityList* collidable_platforms = entity_to_collide->collidable_platforms;
		for (int i = 0; i < collidable_platforms->size(); ++i)
		{
			bool curr_platform_collided = false;
			Entity* platform = collidable_platforms->at(i);
			BoxCollider& other = platform->get_collider();

			// One way platform check
			bool b_istype_oneway = typeid(*platform) == typeid(PlatformerOneWayTile);

			// Check collision bottom if move in that direction
			if (b_istype_oneway == false && curr_fall_vel > 0 && collider.will_touch_bottom(other, curr_fall_vel * deltaTime))
			{
				float to_move_down = other.top - collider.height - collider.top;

				if (to_move_down < 0 == false)
				{
					move(0.f, to_move_down);

					b_y_collided = true;
					curr_fall_vel = 0.f;
					curr_platform_collided = true;
				}
			}
			//if (b_istype_oneway == false && curr_fall_vel < 0 && collider.will_touch_top(other, curr_fall_vel * deltaTime))
			//{
			//	float to_move_up = (other.top + other.height) - collider.top;
			//	
			//	if (to_move_up > 0 == false)
			//	{
			//		move(0.f, to_move_up);

			//		b_y_collided = true;
			//		//curr_fall_vel = 0.f;
			//		curr_platform_collided = true;
			//	}
			//}
			if (curr_platform_collided)
			{
				platform->collided(this);
			}
		}
		if (!b_y_collided)
		{
			if (entity_to_collide != nullptr && entity_to_collide->is_active())
			{
				// move player
				if (collider.will_touch_top(entity_to_collide->get_collider(), 0.01f))
				{
					entity_to_collide->move(0.f, curr_fall_vel * deltaTime);
				}
			}

			// move block
			move(0.f, curr_fall_vel * deltaTime);
		}

		// check if collide with player on bottom, if so, deal damage to player
		if (collider.will_touch_bottom(entity_to_collide->get_collider(), 0.3f))
		{
			entity_to_collide->affect_health(-3.f);
		}
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
			b_anim_paused = false;
		}
	}
}

void PlatformerFallingPlatform::collided(Entity* collided_entity)
{
	if (collided_entity == entity_to_collide 
		&& entity_to_collide->camera 
		&& (curr_fall_vel != 0.f || initial_delay_timer < initial_delay_seconds))		// either platform is moving or been activated
	{
		if (initial_delay_timer < initial_delay_seconds)
		{
			entity_to_collide->camera->shake_camera(0.15f, 0.15f, 0.85f, 0.02f, true);	// small shake before starting to fall
		}
		else
		{
			entity_to_collide->camera->shake_camera(0.4f, 0.4f, 0.85f, 0.02f, true);	// big shake while falling
		}
	}
}

void PlatformerFallingPlatform::launch_up(float yvel_up)
{
	curr_fall_vel = yvel_up;
}
