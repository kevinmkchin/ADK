#include "GrapplehookPlayer.h"
#include "Engine/ADKRaycast.h"
#include "Engine/ADKGameStatics.h"
#include "Engine/EntityList.h"
#include "Engine/ADKMath.h"
#include "PlatformerTriggerBox.h"
#include "PlatformerOneWayTile.h"

ADKOBJECT(GrapplehookPlayer)
GrapplehookPlayer::GrapplehookPlayer()
	: game_window(ADKGameStatics::game_window_static)
{
	ADKOBJECT_BEGIN(GrapplehookPlayer);
	ADKOBJECT_END();

	// Attributes
	max_health = 1.f;
	health = max_health;
	// X vel
	curr_xvel = 0.f;
	max_xvel = 480.f;
	max_xvel_inair = 6000.f;
	max_xvel_inair_by_ad = 500.f;
	max_xvel_decel = 6000.f;
	acc_ground_xvel = 4500.f;
	dec_ground_xvel = 6000.f;
	acc_air_xvel = 2000.f;
	dec_air_xvel = 500.f;
	// Y vel
	curr_yvel = 0.f;
	gravity = 2000.f;
	extra_down_acc = 400.f;
	jump_vel = 500.f;
	max_pos_yvel_default = 1200.f;
	max_pos_yvel = max_pos_yvel_default;
	max_pos_yvel_when_pressing_s = max_pos_yvel_default * 2.f;
	max_pos_yvel_decel = 850.f;
	// Jumping
	b_jumping = false;
	b_pending_jump = false;
	b_jumpkey_down = false;
	jump_peak_yvel = -1.f;
	percent_yvel_on_jump_release = 0.5f;
	// Jump buffering
	jump_buffer_maxhold_seconds = 0.30f;
	jump_buffer_maxtap_seconds = 0.12f;
	jump_buffer_timer_seconds = 0.f;
	// Coyote time
	coyote_time_default_seconds = 0.07f;
	coyote_time_timer_seconds = coyote_time_default_seconds;
	b_input_paused = false;
	input_pause_timer_seconds = 0.f;
	// Launched
	b_launched = false;
	// death grace period
	death_grace_period_default = 1.f;
	death_grace_period_timer = death_grace_period_default;
	// States
	b_try_fall_from_oneway = false;
	b_jumped_from_ground_this_frame = false;
	b_landed_this_frame = false;
	b_intersecting_oneway_lastframe = false;

	set_frame_size(64, 64);

	b_left_mouse_down = false;
	hooked_entity = nullptr;
	// GrappleHookPlayer
	hook_start_offset = sf::Vector2f((float)(sprite_sheet.frame_size.x / 2), (float)(sprite_sheet.frame_size.x / 2));
	hook_max_len = 3000.f;
	hook_pos = sf::Vector2f(0.f, 0.f);

	collider = BoxCollider(get_position().x, get_position().y, 0.5f, 1.f, 63.f, 63.f);
}

void GrapplehookPlayer::update(float deltaTime)
{
	Entity::update(deltaTime);

	// death grace timer / invulernability
	if (death_grace_period_timer > 0.f)
	{
		death_grace_period_timer -= deltaTime;
	}

	read_input(deltaTime);

	resolve_movement(deltaTime);

	// Trigger boxes
	for (int i = 0; i < trigger_boxes->size(); ++i)
	{
		Entity* e = trigger_boxes->at(i);
		if (collider.intersects(e->get_collider()))
		{
			if (PlatformerTriggerBox* damage_block = dynamic_cast<PlatformerTriggerBox*>(e))
			{
				// Trigger the trigger box
				damage_block->collided(this);
			}
		}
	}

	reset_states();
}

void GrapplehookPlayer::render(sf::RenderTarget& target)
{
	Entity::render(target);

	sf::CircleShape hook(10);
	hook.setPosition(hook_pos);
	hook.setFillColor(sf::Color(100, 250, 50));
	target.draw(hook);

	if (b_hook_active)
	{
		sf::Vertex line[] =
		{
			sf::Vertex(get_position() + hook_start_offset),
			sf::Vertex(hook_pos)
		};
		target.draw(line, 2, sf::Lines);
	}
}

void GrapplehookPlayer::move_hook(sf::Vector2f move, Entity* calledby)
{
	if (b_hook_active && calledby == hooked_entity)
	{
		hook_pos += move;
	}
}

void GrapplehookPlayer::read_input(float dt)
{
	// input pause
	if (input_pause_timer_seconds > 0.f)
	{
		input_pause_timer_seconds -= dt;
		if (input_pause_timer_seconds <= 0.f)
		{
			b_input_paused = false;
		}
	}

	// input check
	bool b_j_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && b_input_paused == false;
	bool b_s_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S) && b_input_paused == false;
	bool b_a_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A) && b_input_paused == false;
	bool b_d_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D) && b_input_paused == false;

	// Jumping
	if (b_j_pressed == false)
	{
		if (b_jumpkey_down)
		{
			// jumpkey released
			b_jumpkey_down = false;

			// cut our current yvel up
			if (curr_yvel < 0.f && jump_buffer_timer_seconds == 0.f) // only if we are moving up, and if this is the first time we released jump while jumping
			{
				curr_yvel *= percent_yvel_on_jump_release;
			}
		}
	}
	if (b_j_pressed && b_jumpkey_down == false)
	{
		b_pending_jump = true;
		b_jumpkey_down = true;
		jump_buffer_timer_seconds = 0.f;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (b_left_mouse_down == false)
		{
			b_left_mouse_down = true;

			sf::Vector2i pixel_pos = sf::Mouse::getPosition(*game_window);
			sf::Vector2f world_pos = (*game_window).mapPixelToCoords(pixel_pos);
			Entity* hitguy = nullptr;
			Tags tile_tag = TAG_PLATFORMTILE;
			hook_pos = ADKRaycast::raycast2d(get_position() + hook_start_offset, world_pos, hook_max_len, collidable_platforms, hitguy, &tile_tag);
			hooked_entity = hitguy;

			b_hook_active = hitguy != nullptr;
		}
	}
	else
	{
		sf::Vector2i pixel_pos = sf::Mouse::getPosition(*game_window);
		sf::Vector2f world_pos = (*game_window).mapPixelToCoords(pixel_pos);
		Entity* hitguy = nullptr;
		Tags tile_tag = TAG_PLATFORMTILE;
		hook_pos = ADKRaycast::raycast2d(get_position() + hook_start_offset, world_pos, hook_max_len, collidable_platforms, hitguy, &tile_tag);

		b_left_mouse_down = false;
		b_hook_active = false;
	}

	b_jumping |= b_hook_active;

	// --- Gravity ---
	if (b_jumping && b_launched == false && abs(curr_yvel) <= jump_peak_yvel && b_s_pressed == false)
	{
		/*	Half gravity if jumping, not launched, y velocity is less than jump peak yvel, and we are not pressing s/down. */
		curr_yvel += (gravity / 2) * dt;
	}
	else
	{
		curr_yvel += gravity * dt;
	}

	// --- Hook Physics ---
	if (b_hook_active)
	{
		/*
		# `to_local($Chain.tip).normalized()` is the direction that the chain is pulling


		chain_velocity = to_local($Chain.tip).normalized() * CHAIN_PULL
		if chain_velocity.y > 0:
			# Pulling down isn't as strong
			chain_velocity.y *= 0.55
		else:
			# Pulling up is stronger
			chain_velocity.y *= 1.65
		if sign(chain_velocity.x) != sign(walk):
			# if we are trying to walk in a different
			# direction than the chain is pulling
			# reduce its pull
			chain_velocity.x *= 0.7

		*/

		// Normalized direction grapple hook is pulling
		sf::Vector2f dir = hook_pos - (get_position() + hook_start_offset);
		dir = ADKMath::noramlize_vector(dir);
		hook_force = dir * 2800.f;

		if (hook_force.y > 0.f)
		{
			// Pulling down isn't as strong
			hook_force.y *= 0.6f;
		}
		else
		{
			// Pulling up is stronger
			hook_force.y *= 1.6f;
		}

	}
	else
	{
		hook_force.x = 0.f;
		hook_force.y = 0.f;
	}
	curr_xvel += hook_force.x * dt;
	curr_yvel += hook_force.y * dt;

	// --- Positive Accelerations ---
	if (b_s_pressed)
	{
		max_pos_yvel = max_pos_yvel_when_pressing_s;
		curr_yvel += extra_down_acc * dt;
		// b_try_fall_from_oneway = true; // UNCOMMENT IF YOU WANT TO BE ABLE TO DROP DOWN FROM ONE WAY PLATFORMS
	}
	else
	{
		max_pos_yvel = max_pos_yvel_default;
	}
	if (b_a_pressed)
	{
		float vel_to_subtract = b_jumping ? acc_air_xvel : acc_ground_xvel; // check if we are jumping
		if (curr_xvel > 0.f) // check if we moving in the other direction
		{
			vel_to_subtract += b_jumping ? dec_air_xvel : dec_ground_xvel; // if we are, then we need to decelerate as well
		}
		
		if (curr_xvel > -max_xvel_inair_by_ad)
		{
			curr_xvel -= vel_to_subtract * dt;
		}
	}
	if (b_d_pressed)
	{
		float vel_to_add = b_jumping ? acc_air_xvel : acc_ground_xvel;
		if (curr_xvel < 0.f)
		{
			vel_to_add += b_jumping ? dec_air_xvel : dec_ground_xvel;
		}

		if (curr_xvel < max_xvel_inair_by_ad)
		{
			curr_xvel += vel_to_add * dt;
		}
	}

	// Negative Accelerations
	if (b_a_pressed == false && b_d_pressed == false)
	{
		if (curr_xvel > 0.f)
		{
			curr_xvel -= (b_jumping ? dec_air_xvel : dec_ground_xvel) * dt;
		}
		else if (curr_xvel < 0.f)
		{
			curr_xvel += (b_jumping ? dec_air_xvel : dec_ground_xvel) * dt;
		}

		// curr_xvel is hella small, just set it to 0
		if (abs(curr_xvel) <= (b_jumping ? dec_air_xvel : dec_ground_xvel) * dt)
		{
			curr_xvel = 0.f;
		}
	}

	// Cap curr_yvel
	if (curr_yvel > max_pos_yvel)
	{
		curr_yvel -= max_pos_yvel_decel * dt;
	}
	// Cap curr_xvel to max_xvel if we are grounded
	float xvel_cap = b_hook_active ? max_xvel_inair : (b_jumping ? max_xvel_inair_by_ad : max_xvel);
	if (abs(curr_xvel) > xvel_cap)
	{
		if (curr_xvel > 0)
		{
			curr_xvel -= max_xvel_decel * dt;
		}
		else
		{
			curr_xvel += max_xvel_decel * dt;
		}
	}
}

void GrapplehookPlayer::resolve_movement(float dt)
{
	bool b_x_collided = false;
	bool b_y_collided = false;
	bool b_is_grounded = false;
	bool b_col_directly_above = false;
	bool b_jumped_this_frame = false;

	bool b_intersecting_oneway = false;

	// --- Figure out our current situation ---
	for (int i = 0; i < collidable_platforms->size(); ++i)
	{
		BoxCollider& other = collidable_platforms->at(i)->get_collider();

		// one way platform check
		bool b_istype_oneway = typeid(*(collidable_platforms->at(i))) == typeid(PlatformerOneWayTile);

		if (b_istype_oneway)
		{
			// check if we are inside the one way platform 
			if (b_intersecting_oneway == false)
			{
				b_intersecting_oneway = collider.intersects(other);
			}
		}
	}

	// --- Check movement collision ---
	for (int i = 0; i < collidable_platforms->size(); ++i)
	{
		bool curr_platform_collided = false;
		Entity* platform = collidable_platforms->at(i);
		BoxCollider& other = platform->get_collider();

		// One way platform check
		bool b_istype_oneway = typeid(*platform) == typeid(PlatformerOneWayTile);

		// Check if grounded or hitting ceiling
		if (collider.will_touch_bottom(other, 0.01f))
		{
			b_is_grounded = true;
			curr_platform_collided = true;
		}
		if (collider.will_touch_top(other, 0.01f))
		{
			b_col_directly_above = true;
			curr_platform_collided = true;
		}

		// Jump
		if (b_pending_jump && b_col_directly_above == false)
		{
			if (b_is_grounded || (coyote_time_timer_seconds > 0.f && b_jumping == false)) // Not grounded is fine if within coyote time
			{
				b_pending_jump = false;
				curr_yvel = -jump_vel;
				b_jumping = true;
				b_launched = false;
				b_jumped_this_frame = true;
				b_jumped_from_ground_this_frame = true;
			}
		}

		// Down collision
		if (b_jumped_this_frame == false					// don't check bottom collision on the frame we jump
			&& b_intersecting_oneway_lastframe == false		// don't check bottom collision if we were already intersecting a one way platform last frame
			&& (b_try_fall_from_oneway == false || b_intersecting_oneway == false))
		{
			// Check collision bottom if move in that direction
			if (curr_yvel > 0 && collider.will_touch_bottom(other, curr_yvel * dt))
			{
				float to_move_down = other.top - collider.height - collider.top;

				if (to_move_down < 0 == false || b_intersecting_oneway == false)
				{
					move(0.f, to_move_down);

					// On landed from falling or jumping
					if (b_jumping && b_launched_this_frame == false)
					{
						b_jumping = false;
					}
					if (curr_yvel > (max_pos_yvel * dust_kickup_maxyvel_percent))
					{
						b_landed_this_frame = true;
					}

					b_y_collided = true;
					curr_yvel = 0.f;
					curr_platform_collided = true;
				}
			}
		}
		// Up Right Left collision
		if (b_istype_oneway == false)
		{
			// Check collision top if move in that direction
			if (curr_yvel < 0 && collider.will_touch_top(other, curr_yvel * dt))
			{
				float to_move_up = (other.top + other.height) - collider.top;

				move(0.f, to_move_up);

				b_y_collided = true;
				curr_yvel = 0.f;
				curr_platform_collided = true;
			}
			// Check collision right if move in that direction
			if (curr_xvel > 0 && collider.will_touch_right(other, curr_xvel * dt))
			{
				float to_move_right = other.left - (collider.left + collider.width);

				move(to_move_right, 0.f);

				b_x_collided = true;
				curr_xvel = 0.f;
				curr_platform_collided = true;
			}
			// Check collision left if move in that direction
			if (curr_xvel < 0 && collider.will_touch_left(other, curr_xvel * dt))
			{
				float to_move_left = (other.left + other.width) - collider.left;

				move(to_move_left, 0.f);

				b_x_collided = true;
				curr_xvel = 0.f;
				curr_platform_collided = true;
			}
		}

		if (curr_platform_collided)
		{
			platform->collided(this);
		}
	}

	// --- Move ---
	if (!b_x_collided)
	{
		move(curr_xvel * dt, 0.f);
	}
	if (!b_y_collided)
	{
		move(0.f, curr_yvel * dt);
	}

	// --- Reset flags and timers ---

	// Coyote time
	if (b_is_grounded) // if grounded, then coyote timer should be at default value
	{
		coyote_time_timer_seconds = coyote_time_default_seconds;
	}
	else if (b_jumping == false) // if we aren't grounded or jumping, then we must be falling
	{
		coyote_time_timer_seconds -= dt;
	}

	// Jump buffering and b_pending_jump
	if (b_pending_jump)
	{
		jump_buffer_timer_seconds += dt; // tick the jump buffer timer
	}
	if (b_pending_jump && b_jumpkey_down == false && jump_buffer_timer_seconds > jump_buffer_maxtap_seconds)
	{
		b_pending_jump = false; // not pending jump anymore if we aren't holding jumpkey and past jump_buffer_maxtap_seconds
	}
	if (b_pending_jump && jump_buffer_timer_seconds > jump_buffer_maxhold_seconds)
	{
		b_pending_jump = false; // not pending jump anymore if we are past the jump_buffer_maxhold_seconds
	}

	// Remember states for next frame
	b_intersecting_oneway_lastframe = b_intersecting_oneway;
	if (b_y_collided) // if we collided in y direction, that means our y has been corrected and we are no longer colliding with a oneway platform this frame
	{
		b_intersecting_oneway_lastframe = false;
	}
}

void GrapplehookPlayer::reset_states()
{
	// Reset states
	b_try_fall_from_oneway = false;
	b_jumped_from_ground_this_frame = false;
	b_landed_this_frame = false;
	b_launched_this_frame = false;
}
