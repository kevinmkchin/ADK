#include "PlatformerPlayer.h"
#include "Engine/EntityList.h"

#include "DemoPlatformerOneWayPlatform.h"
#include "PlatformerSpikes.h"

PlatformerPlayer::PlatformerPlayer()
	: max_health(1.f)
	, health(max_health)
	, curr_xvel(0.f)
	, max_xvel(80.f)
	, max_xvel_inair(110.f)
	, acc_ground_xvel(700.f)
	, dec_ground_xvel(1000.f)
	, acc_air_xvel(350.f)
	, dec_air_xvel(400.f)
	, curr_yvel(0.f)
	, gravity(600.f)
	, extra_down_acc(275.f)
	, jump_vel(170.f)
	, max_pos_yvel(400.f)
	, b_jumping(false)
	, b_pending_jump(false)
	, b_jumpkey_down(false)
	, jump_peak_yvel(50.f)
	, percent_yvel_on_jump_release(0.5f)
	// Jump buffering
	, jump_buffer_maxhold_seconds(0.30f)
	, jump_buffer_maxtap_seconds(0.12f)
	, jump_buffer_timer_seconds(0.f)
	// Coyote time
	, coyote_time_default_seconds(0.07f)
	, coyote_time_timer_seconds(coyote_time_default_seconds)
	// States
	, b_try_fall_from_oneway(false)
	, b_intersecting_oneway_lastframe(false)
{
	collidable_platforms = new EntityList();

	set_frame_size(8, 13);

	init_collider();
}

void PlatformerPlayer::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 0.5f, 1.f, 7.f, 12.f);
}

void PlatformerPlayer::load_default_texture()
{
	texture_path = "Game/style1/black16.png";
	Entity::load_default_texture();
}

void PlatformerPlayer::affect_health(float delta)
{
	health += delta;

	if (health <= 0.f)
	{
		die();
	}
}

void PlatformerPlayer::launch_up(float in_yvel)
{
	// set yvel so we go up
	curr_yvel = in_yvel;

	// set to jumping i guess
	bool b_jumping = true;
}

void PlatformerPlayer::begin_play()
{
	load_default_texture();
	Entity::begin_play();
}

void PlatformerPlayer::update(float deltaTime)
{
	Entity::update(deltaTime);

	read_input(deltaTime);

	resolve_movement(deltaTime);

	// Damage blocks
	for (int i = 0; i < damage_blocks->size(); ++i)
	{
		Entity* e = damage_blocks->at(i);
		
		if (collider.intersects(e->get_collider()))
		{
			// Spike
			if (PlatformerSpikes* spike = dynamic_cast<PlatformerSpikes*>(e))
			{
				spike->collided(this);
			}


		}
	}
}

void PlatformerPlayer::read_input(float dt)
{
	bool b_j_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
	bool b_s_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	bool b_a_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	bool b_d_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

	//bool b_j_pressed = sf::Joystick::isButtonPressed(1, 0);//sf::Keyboard::isKeyPressed(sf::Keyboard::J);
	//bool b_s_pressed = sf::Joystick::isButtonPressed(1, 8);//sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	//bool b_a_pressed = false;//sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	//bool b_d_pressed = false;//sf::Keyboard::isKeyPressed(sf::Keyboard::D);

	float pov_x_input = sf::Joystick::getAxisPosition(1, sf::Joystick::PovX);
	if (pov_x_input > 0.4f)
	{
		b_d_pressed = true;
	}
	if (pov_x_input < -0.4f)
	{
		b_a_pressed = true;
	}



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

	// --- Gravity ---
	if (b_jumping && abs(curr_yvel) <= jump_peak_yvel && b_s_pressed == false)
	{
		curr_yvel += (gravity / 2) * dt;
	}
	else
	{
		curr_yvel += gravity * dt;
	}
	
	// --- Positive Accelerations ---
	if (b_s_pressed)
	{
		curr_yvel += extra_down_acc * dt;
		// b_try_fall_from_oneway = true; // UNCOMMENT IF YOU TO BE ABLE TO DROP DOWN FROM ONE WAY PLATFORMS
	}
	if (b_a_pressed)
	{
		float vel_to_subtract = b_jumping ? acc_air_xvel : acc_ground_xvel; // check if we are jumping
		if (curr_xvel > 0.f) // check if we moving in the other direction
		{
			vel_to_subtract += b_jumping ? dec_air_xvel : dec_ground_xvel; // if we are, then we need to decelerate as well
		}

		curr_xvel -= vel_to_subtract * dt;
	}
	if (b_d_pressed)
	{
		float vel_to_add = b_jumping ? acc_air_xvel : acc_ground_xvel;
		if (curr_xvel < 0.f)
		{
			vel_to_add += b_jumping ? dec_air_xvel : dec_ground_xvel;
		}

		curr_xvel += vel_to_add * dt;
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
		if (abs(curr_xvel) <= dec_ground_xvel * dt)
		{
			curr_xvel = 0.f;
		}
	}

	// Cap curr_yvel
	if (curr_yvel > max_pos_yvel)
	{
		curr_yvel = max_pos_yvel;
	}
	// Cap curr_xvel to max_xvel if we are grounded
	float xvel_cap = b_jumping ? max_xvel_inair : max_xvel;
	if (abs(curr_xvel) > xvel_cap)
	{
		curr_xvel = xvel_cap * (curr_xvel / abs(curr_xvel));
	}

}

void PlatformerPlayer::resolve_movement(float dt)
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
		bool b_istype_oneway = typeid(*(collidable_platforms->at(i))) == typeid(DemoPlatformerOneWayPlatform);

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
		bool b_istype_oneway = typeid(*platform) == typeid(DemoPlatformerOneWayPlatform);

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
				b_jumped_this_frame = true;
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

					b_jumping = false;
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

	// Reset states
	b_try_fall_from_oneway = false;

	// Remember states for next frame
	b_intersecting_oneway_lastframe = b_intersecting_oneway;
	if (b_y_collided) // if we collided in y direction, that means our y has been corrected and we are no longer colliding with a oneway platform this frame
	{
		b_intersecting_oneway_lastframe = false;
	}
}

void PlatformerPlayer::die()
{
	printf("die\n");
	set_position(100.f, 100.f);
}
