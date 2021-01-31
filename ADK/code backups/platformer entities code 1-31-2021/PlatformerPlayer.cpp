#include <stdlib.h>

#include "PlatformerPlayer.h"
#include "Engine/EntityList.h"
#include "Engine/ADKCamera.h"
#include "Scene_PlatformerGame.h"

#include "PlatformerOneWayTile.h"
#include "PlatformerTriggerBox.h"
#include "PlatformerTrampoline.h"

PlatformerPlayer::PlatformerPlayer()
	: collidable_platforms(new EntityList())
	, trigger_boxes(new EntityList())
	, camera(nullptr)
	, owning_scene(nullptr)

	// Attributes
	, max_health(1.f)
	, health(max_health)

	// X vel
	, curr_xvel(0.f)
	, max_xvel(80.f)
	, max_xvel_inair(80.f)
	, max_xvel_decel(1000.f)
	, acc_ground_xvel(700.f)
	, dec_ground_xvel(1000.f)
	, acc_air_xvel(540.f)
	, dec_air_xvel(200.f)

	// Y vel
	, curr_yvel(0.f)
	, gravity(600.f)
	, extra_down_acc(275.f)
	, jump_vel(170.f)
	, max_pos_yvel_default(155.f)
	, max_pos_yvel(max_pos_yvel_default)
	, max_pos_yvel_when_pressing_s(max_pos_yvel_default * 2.f)
	, max_pos_yvel_decel(1400.f)

	// Jumping
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
	, b_input_paused(false)
	, input_pause_timer_seconds(0.f)

	// Launched
	, b_launched(false)

	// death grace period
	, death_grace_period_default(1.f)
	, death_grace_period_timer(death_grace_period_default)

	// States
	, b_try_fall_from_oneway(false)
	, b_jumped_from_ground_this_frame(false)
	, b_landed_this_frame(false)
	, b_intersecting_oneway_lastframe(false)


	// Dust effects
	, dust_tick_interval_seconds(0.05f)
	, dust_dissipation_chance(30)
	, dust_move_up_chance(28)
	, dust_move_sideways_chance(10)

	, dust_lifetime_jump(0.42f)
	, dust_num_pixels_min_jump(48)
	, dust_num_pixels_max_jump(68)
	, dust_down_adjustment_jump(3)
	, dust_width_min_jump(7)
	, dust_width_variance_jump(3)
	, dust_height_min_jump(2)
	, dust_height_variance_jump(2)

	, dust_lifetime_land(0.62f)
	, dust_num_pixels_min_land(48)
	, dust_num_pixels_max_land(68)
	, dust_down_adjustment_land(0)
	, dust_width_min_land(8)
	, dust_width_variance_land(4)
	, dust_height_min_land(3)
	, dust_height_variance_land(3)
	, dust_kickup_maxyvel_percent(0.3f)

	, dust_group_size(dust_num_pixels_max_jump > dust_num_pixels_max_land ? dust_num_pixels_max_jump : dust_num_pixels_max_land)
{
	texture_path = "Misc/style1/black16.png";

	set_frame_size(8, 13);

	init_collider();
}

PlatformerPlayer::~PlatformerPlayer()
{
	delete collidable_platforms;
	delete trigger_boxes;
}

void PlatformerPlayer::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 0.5f, 1.f, 7.f, 12.f);
}

void PlatformerPlayer::affect_health(float delta)
{
	if (health + delta <= 0.f) //if we are going to die
	{
		if (death_grace_period_timer > 0.f)	// if we are within death grace period, don't take damage
		{
			return;
		}
	}

	health += delta;
	if (health <= 0.f)
	{
		die();
	}
}

void PlatformerPlayer::launch(float in_xvel, float in_yvel, float input_pause)
{
	// set xvel
	curr_xvel = in_xvel;
	// set yvel so we go up
	curr_yvel = in_yvel;

	// set to jumping i guess
	b_jumping = true;
	b_launched_this_frame = true;
	b_launched = true;

	// camera shake
	if (camera != nullptr)
	{
		if (in_xvel != 0.f)
		{
			camera->shake_camera(3.5f, 0.f, 0.6f, 0.8f, true);
		}
		else
		{
			camera->shake_camera(0.f, 3.5f, 0.6f, 0.8f, true);
		}
	}

	// pause inputs when we just got launched and input_pause > 0.f
	if (input_pause > 0.f)
	{
		b_input_paused = true;
		input_pause_timer_seconds = input_pause;
	}
}

void PlatformerPlayer::restart_player()
{
	set_position(50.f, 5.f);

	curr_xvel = 0.f;
	curr_yvel = 0.f;

	set_active(true);
}

void PlatformerPlayer::begin_play()
{
	load_default_texture();
	Entity::begin_play();
}

void PlatformerPlayer::update(float deltaTime)
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

	update_dust_effects(deltaTime);

	reset_states();
}

void PlatformerPlayer::render(sf::RenderTarget& target)
{
	Entity::render(target);

	//dust effects
	for (const auto& dust : dust_groups)
	{
		for (const sf::RectangleShape* dust_pixel : dust.dust_effects)
		{
			if (dust_pixel)
			{
				target.draw(*dust_pixel);
			}
		}
	}
}

void PlatformerPlayer::read_input(float dt)
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
	bool b_j_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::J) && b_input_paused == false;
	bool b_s_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S) && b_input_paused == false;
	bool b_a_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A) && b_input_paused == false;
	bool b_d_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D) && b_input_paused == false;

	//bool b_j_pressed = sf::Joystick::isButtonPressed(1, 0);//sf::Keyboard::isKeyPressed(sf::Keyboard::J);
	//bool b_s_pressed = sf::Joystick::isButtonPressed(1, 8);//sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	//bool b_a_pressed = false;//sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	//bool b_d_pressed = false;//sf::Keyboard::isKeyPressed(sf::Keyboard::D);
	//float pov_x_input = sf::Joystick::getAxisPosition(1, sf::Joystick::PovX);
	//if (pov_x_input > 0.4f)
	//{
	//	b_d_pressed = true;
	//}
	//if (pov_x_input < -0.4f)
	//{
	//	b_a_pressed = true;
	//}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
	{
		//curr_xvel = 300.f;
		curr_yvel = -200.f;
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
	if (b_jumping && b_launched == false && abs(curr_yvel) <= jump_peak_yvel && b_s_pressed == false)
	{
		/*	Half gravity if jumping, not launched, y velocity is less than jump peak yvel, and we are not pressing s/down. */
		curr_yvel += (gravity / 2) * dt;
	}
	else
	{
		curr_yvel += gravity * dt;
	}

	// --- Positive Accelerations ---
	if (b_s_pressed)
	{
		max_pos_yvel = max_pos_yvel_when_pressing_s;
		curr_yvel += extra_down_acc * dt;
		// b_try_fall_from_oneway = true; // UNCOMMENT IF YOU TO BE ABLE TO DROP DOWN FROM ONE WAY PLATFORMS
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
		curr_yvel -= max_pos_yvel_decel * dt;
	}
	// Cap curr_xvel to max_xvel if we are grounded
	float xvel_cap = b_jumping ? max_xvel_inair : max_xvel;
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

void PlatformerPlayer::die()
{
	death_grace_period_timer = death_grace_period_default;

	set_active(false);

	if (owning_scene)
	{
		owning_scene->on_player_death();
	}
}

void PlatformerPlayer::update_dust_effects(float dt)
{
	// --- Add new dust groups (new dust kicked up) ---
	// from jump
	if (b_jumped_from_ground_this_frame && coyote_time_timer_seconds == coyote_time_default_seconds)
	{
		int px = (int)(get_position().x + (int)((float)sprite_sheet.frame_size.x / 2.f));
		int py = (int)(get_position().y + ((float)sprite_sheet.frame_size.y) + (float)dust_down_adjustment_jump);

		DustGroup new_dust;
		new_dust.dust_life_timer = dust_lifetime_jump;
		new_dust.dust_tick_timer_default = dust_tick_interval_seconds;
		new_dust.dust_tick_timer = new_dust.dust_tick_timer_default;
		for (int i = 0; i < dust_group_size; ++i)
		{
			new_dust.dust_effects.push_back(nullptr);
		}

		// num pixels to create
		int num_pixels = dust_num_pixels_min_jump + (rand() % (dust_num_pixels_max_jump - dust_num_pixels_min_jump + 1));
		for (int i = 0; i < num_pixels; ++i)
		{
			int x_var_variance = dust_width_min_jump + (rand() % dust_width_variance_jump);
			int y_var_variance = dust_height_min_jump + (rand() % dust_height_variance_jump);
			int x_var = -(x_var_variance / 2) + (rand() % x_var_variance);
			int y_var = -(y_var_variance / 2) + (rand() % y_var_variance);

			sf::RectangleShape* dust_pixel = new sf::RectangleShape(sf::Vector2f(1.f, 1.f));
			dust_pixel->setFillColor(sf::Color::White);
			dust_pixel->setPosition((float) (px + x_var), (float) (py + y_var));

			new_dust.dust_effects[i] = dust_pixel;
		}

		dust_groups.push_back(new_dust);
	}

	// from landing
	if (b_landed_this_frame && b_launched_this_frame == false)
	{
		int px = (int)(get_position().x + (int)((float)sprite_sheet.frame_size.x / 2.f));
		int py = (int)(get_position().y + ((float)sprite_sheet.frame_size.y) + (float)dust_down_adjustment_land);

		DustGroup new_dust;
		new_dust.dust_life_timer = dust_lifetime_land;
		new_dust.dust_tick_timer_default = dust_tick_interval_seconds;
		new_dust.dust_tick_timer = new_dust.dust_tick_timer_default;
		for (int i = 0; i < dust_group_size; ++i)
		{
			new_dust.dust_effects.push_back(nullptr);
		}

		// num pixels to create
		int num_pixels = dust_num_pixels_min_land + (rand() % (dust_num_pixels_max_land - dust_num_pixels_min_land + 1));
		for (int i = 0; i < num_pixels; ++i)
		{
			int x_var_variance = dust_width_min_land + (rand() % dust_width_variance_land);
			int y_var_variance = dust_height_min_land + (rand() % dust_height_variance_land);
			int x_var = -(x_var_variance / 2) + (rand() % x_var_variance);
			int y_var = -(y_var_variance / 2) + (rand() % y_var_variance);

			sf::RectangleShape* dust_pixel = new sf::RectangleShape(sf::Vector2f(1.f, 1.f));
			dust_pixel->setFillColor(sf::Color::White);
			dust_pixel->setPosition((float)(px + x_var), (float)(py + y_var));

			new_dust.dust_effects[i] = dust_pixel;
		}

		dust_groups.push_back(new_dust);
	}

	// --- Update dust and dust pixels ---
	std::vector<std::size_t> indices_to_remove;
	for (std::size_t i = 0; i < dust_groups.size(); ++i)
	{
		DustGroup& dust = dust_groups[i];
		dust.dust_life_timer -= dt;
		if (dust.dust_life_timer <= 0.f) // kill dust
		{
			indices_to_remove.push_back(i);
		}
		else // move dust
		{
			dust.dust_tick_timer -= dt;
			if (dust.dust_tick_timer <= 0.f)
			{
				std::vector<int> pixel_indices_to_remove;
				for (int j = 0; j < dust_group_size; ++j)
				{
					sf::RectangleShape* pixel = dust.dust_effects[j];
					if (pixel == nullptr)
					{
						continue;
					}

					bool b_die = ((float)(rand() % 100)) < dust_dissipation_chance;
					if (b_die)
					{
						pixel_indices_to_remove.push_back(j);
						continue;
					}
					bool b_moveup = ((float)(rand() % 100)) < dust_move_up_chance;
					if (b_moveup)
					{
						pixel->setPosition(pixel->getPosition().x, pixel->getPosition().y - 1.f);
					}
					bool b_moveleftorright = ((float)(rand() % 100)) < dust_move_sideways_chance;
					if (b_moveleftorright)
					{
						bool b_moveleft = rand() % 2;
						pixel->setPosition(pixel->getPosition().x + (b_moveleft ? -1.f : 1.f), pixel->getPosition().y);
					}
				}
				for (int j : pixel_indices_to_remove)
				{
					delete dust.dust_effects[j];
					dust.dust_effects[j] = nullptr;
				}

				dust.dust_tick_timer = dust.dust_tick_timer_default;
			}
		}
	}

	// Remove dust if dust_life_timer is <= 0
	int num_removed = 0;
	for (std::size_t i : indices_to_remove)
	{
		i -= num_removed;
		for (int j = 0; j < dust_group_size; ++j)
		{
			sf::RectangleShape* pixel = dust_groups[i].dust_effects[j];
			if (pixel)
			{
				delete pixel;
			}
		}
		dust_groups.erase(dust_groups.begin() + i);
		++num_removed;
	}
}

void PlatformerPlayer::reset_states()
{
	// Reset states
	b_try_fall_from_oneway = false;
	b_jumped_from_ground_this_frame = false;
	b_landed_this_frame = false;
	b_launched_this_frame = false;
}
