#include "PlatformerPlayer.h"
#include "../Engine/EntityList.h"

PlatformerPlayer::PlatformerPlayer()
	: curr_xvel(0.f)
	, max_xvel(250.f)
	, acc_ground_xvel(800.f)
	, dec_ground_xvel(800.f)
	, acc_air_xvel(2000.f)
	, dec_air_xvel(500.f)
	, curr_yvel(0.f)
	, gravity(400.f)
	, extra_down_acc(550.f)
	, jump_vel(200.f)
	, max_pos_yvel(800.f)
	, b_jumping(false)
	, b_pending_jump(false)
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
	bool b_jump_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
	bool b_s_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	bool b_a_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
	bool b_d_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

	// Jumping
	if (b_jump_pressed == false && b_jumpkey_pressed)
	{
		b_jumpkey_pressed = false;
	}
	if (b_jump_pressed && b_jumping == false && b_jumpkey_pressed == false)
	{
		b_pending_jump = true;
		b_jumpkey_pressed = true;
	}

	// Gravity
	curr_yvel += gravity * dt;
	
	// Positive Accelerations
	if (b_s_pressed)
	{
		curr_yvel += extra_down_acc * dt;
		// TODO GAME to make movement interesting, holding s could make air decceleration slower and air acceleration faster
	}
	if (b_a_pressed)
	{
		if (curr_xvel > 0.f)
		{
			curr_xvel -= (dec_ground_xvel + acc_ground_xvel) * dt;
		}
		else
		{
			curr_xvel -= acc_ground_xvel * dt;
		}
	}
	if (b_d_pressed)
	{
		if (curr_xvel < 0.f)
		{
			curr_xvel += (dec_ground_xvel + acc_ground_xvel) * dt;
		}
		else
		{
			curr_xvel += acc_ground_xvel * dt;
		}
	}

	// Negative Accelerations
	if (b_a_pressed == false && b_d_pressed == false)
	{
		if (curr_xvel > 0.f)
		{
			curr_xvel -= dec_ground_xvel * dt;
		}
		else if (curr_xvel < 0.f)
		{
			curr_xvel += dec_ground_xvel * dt;
		}

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
	// Cap curr_xvel to max_xvel
	if (abs(curr_xvel) > max_xvel)
	{
		curr_xvel = max_xvel * (curr_xvel / abs(curr_xvel));
	}
}

void PlatformerPlayer::resolve_movement(float dt)
{
	bool b_x_collided = false;
	bool b_y_collided = false;
	bool b_is_grounded = false;
	bool b_col_directly_above = false;

	//check movement collision then move
	for (int i = 0; i < collidable_platforms->size(); ++i) 
	{
		BoxCollider& other = collidable_platforms->at(i)->GetCollider();

		if (collider.will_touch_bottom(other, 0.01f)) 
		{
			b_is_grounded = true;
		} 
		else 
		{
			b_is_grounded = false;
		}

		if (collider.will_touch_top(other, 0.01f))
		{
			b_col_directly_above = true;
		}

		if (b_pending_jump && b_col_directly_above == false) 
		{
			if (b_is_grounded) 
			{
				curr_yvel = -jump_vel;
				b_jumping = true;
			}
		}

		if (curr_yvel > 0 && collider.will_touch_bottom(other, curr_yvel * dt))
		{
			float to_move_down = other.top - collider.height - collider.top;

			Move(0.f, to_move_down);

			b_jumping = false;
			b_y_collided = true;
		}

		if (curr_yvel < 0 && collider.will_touch_top(other, curr_yvel * dt))
		{
			float to_move_up = (other.top + other.height) - collider.top;

			Move(0.f, to_move_up);

			b_y_collided = true;
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

	if (!b_x_collided) 
	{
		Move(curr_xvel * dt, 0.f);
	}

	if (!b_y_collided) 
	{
		Move(0.f, curr_yvel * dt);
	} else 
	{
		curr_yvel = 0.f;
	}


	// Reset flags
	b_pending_jump = false;
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