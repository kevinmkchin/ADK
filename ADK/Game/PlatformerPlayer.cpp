#include "PlatformerPlayer.h"
#include "../Engine/EntityList.h"

PlatformerPlayer::PlatformerPlayer()
	: curr_xvel(0.f)
	, max_xvel(160.f)
	, max_xvel_inair(230.f)
	, acc_ground_xvel(1400.f)
	, dec_ground_xvel(2000.f)
	, acc_air_xvel(700.f)
	, dec_air_xvel(800.f)
	, curr_yvel(0.f)
	, gravity(1200.f)
	, extra_down_acc(550.f)
	, jump_vel(340.f)
	, max_pos_yvel(800.f)
	, b_jumping(false)
	, b_pending_jump(false)
	, b_jumpkey_down(false)
	, jump_peak_yvel(100.f)
	, percent_yvel_on_jump_release(0.5f)
	// Jump buffering
	, jump_buffer_maxhold_seconds(0.30f)
	, jump_buffer_maxtap_seconds(0.12f)
	, jump_buffer_timer_seconds(0.f)
	// Coyote time
	, coyote_time_default_seconds(0.07f)
	, coyote_time_timer_seconds(coyote_time_default_seconds)
{
	collidable_platforms = new EntityList();
	SetTexturePathAndLoad("Game/black16.png");
	InitCollider();
}

void PlatformerPlayer::InitCollider()
{
	collider = BoxCollider(GetPosition().x, GetPosition().y, 16.f, 16.f);
}

void PlatformerPlayer::Update(float deltaTime)
{
	Entity::Update(deltaTime);

	read_input(deltaTime);

	resolve_movement(deltaTime);

	//resolve_outstanding_collisions(deltaTime);
}

void PlatformerPlayer::read_input(float dt)
{
	bool b_j_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
	bool b_s_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	bool b_a_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	bool b_d_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

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
		// TODO GAME to make movement interesting, holding s could make air deceleration slower and air acceleration faster
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

	// --- Check movement collision ---
	for (int i = 0; i < collidable_platforms->size(); ++i) 
	{
		BoxCollider& other = collidable_platforms->at(i)->GetCollider();

		if (collider.will_touch_bottom(other, 0.01f))
		{
			b_is_grounded = true;
		}

		if (collider.will_touch_top(other, 0.01f))
		{
			b_col_directly_above = true;
		}

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

		if (b_jumped_this_frame == false)
		{
			if (curr_yvel > 0 && collider.will_touch_bottom(other, curr_yvel * dt))
			{
				float to_move_down = other.top - collider.height - collider.top;

				Move(0.f, to_move_down);

				b_jumping = false;
				b_y_collided = true;
				curr_yvel = 0.f;
			}
		}

		if (curr_yvel < 0 && collider.will_touch_top(other, curr_yvel * dt))
		{
			float to_move_up = (other.top + other.height) - collider.top;

			Move(0.f, to_move_up);

			b_y_collided = true;
			curr_yvel = 0.f;
		}

		if (curr_xvel > 0 && collider.will_touch_right(other, curr_xvel * dt))
		{
			float to_move_right = other.left - (collider.left + collider.width);

			Move(to_move_right, 0.f);

			b_x_collided = true;
			curr_xvel = 0.f;
		}

		if (curr_xvel < 0 && collider.will_touch_left(other, curr_xvel * dt)) 
		{
			float to_move_left = (other.left + other.width) - collider.left;

			Move(to_move_left, 0.f);

			b_x_collided = true;
			curr_xvel = 0.f;
		}
	}

	// --- Move ---
	if (!b_x_collided) 
	{
		Move(curr_xvel * dt, 0.f);
	}
	if (!b_y_collided) 
	{
		Move(0.f, curr_yvel * dt);
	}

	// --- Reset flags and timers ---
	// coyote time
	if (b_is_grounded)
	{
		coyote_time_timer_seconds = coyote_time_default_seconds;
	}
	else
	{
		if (b_jumping == false)
		{
			coyote_time_timer_seconds -= dt;
		}
	}
	// b_pending_jump and jump buffering
	if (b_pending_jump)
	{
		jump_buffer_timer_seconds += dt;
	}
	if (b_pending_jump && b_jumpkey_down == false && jump_buffer_timer_seconds > jump_buffer_maxtap_seconds)
	{
		b_pending_jump = false; // not pending jump anymore if we aren't holding jumpkey and past jump_buffer_maxtap_seconds
	}
	if (b_pending_jump)
	{
		if (jump_buffer_timer_seconds > jump_buffer_maxhold_seconds)
		{
			b_pending_jump = false;
		}
	}
}

void PlatformerPlayer::resolve_outstanding_collisions(float dt)
{
	std::vector<BoxCollider> collided_boxcolliders;
	std::vector<BoxCollider> x_collided_boxcolliders;
	std::vector<BoxCollider> y_collided_boxcolliders;

	// Find boxes we collided with on x or y axis
	for (int i = 0; i < collidable_platforms->size(); ++i)
	{
		BoxCollider& box = collidable_platforms->at(i)->GetCollider();
		if (GetCollider().intersects(box))
		{
			collided_boxcolliders.push_back(box);
		}
	}

	for (BoxCollider box : collided_boxcolliders)
	{
		sf::Vector2f pos_correction = collider.resolve_collision_rect(box);

		if (pos_correction.x != 0)
		{
			x_collided_boxcolliders.push_back(box);
		}
		if (pos_correction.y != 0)
		{
			y_collided_boxcolliders.push_back(box);
		}
	}

	// Resolve x collisions
	for (BoxCollider box : x_collided_boxcolliders)
	{
		sf::Vector2f pos_correction = collider.resolve_collision_rect(box);

		Move(pos_correction.x, 0.f);
	}

	// Resolve y collisions
	for (BoxCollider box : y_collided_boxcolliders)
	{
		sf::Vector2f pos_correction = collider.resolve_collision_rect(box);

		Move(0.f, pos_correction.y);
	}
}