#include "PlatformerTrampoline.h"
#include "PlatformerPlayer.h"
#include "PlatformerFallingPlatform.h"

PlatformerTrampoline::PlatformerTrampoline()
	: launch_vel_y(300.f)
	, launch_vel_x(400.f)
{
	texture_path = "Game/trampoline.png";

	set_frame_size(16, 16);

	sprite_sheet.animations[0].anim_duration = sf::seconds(1.f);
	sprite_sheet.animations[0].num_frames = 16;
	sprite_sheet.b_repeat = false;
	b_anim_paused = true;

	init_collider();
}

void PlatformerTrampoline::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 1.f, 12.f, 14.f, 4.f);
}

void PlatformerTrampoline::set_rotation(float newRot, bool bAffectCollider /*= false*/)
{
	sprite_sheet.sprite.setRotation(newRot);

	if (newRot == 0.f)
	{
		collider.offset_x = 1.f;
		collider.offset_y = 12.f;
		collider.width = 14.f;
		collider.height = 4.f;
	}
	else if (newRot == 90.f)
	{
		collider.offset_x = 0.f;
		collider.offset_y = 1.f;
		collider.width = 4.f;
		collider.height = 14.f;
	}
	else if (newRot == 180.f)
	{
		collider.offset_x = 1.f;
		collider.offset_y = 0.f;
		collider.width = 14.f;
		collider.height = 4.f;
	}
	else if (newRot == 270.f)
	{
		collider.offset_x = 12.f;
		collider.offset_y = 1.f;
		collider.width = 4.f;
		collider.height = 14.f;
	}
}

void PlatformerTrampoline::collided(Entity* collided_entity)
{
	// Bounce player
	if (PlatformerPlayer* player = dynamic_cast<PlatformerPlayer*>(collided_entity))
	{
		if (get_rotation() == 0.f)
		{
			player->launch(0.f, -launch_vel_y);
		}
		else if (get_rotation() == 90.f)
		{
			player->launch(launch_vel_x, -160.f, 0.1f);
		}
		else if (get_rotation() == 180.f)
		{
			player->launch(0.f, launch_vel_y);
		}
		else if (get_rotation() == 270.f)
		{
			player->launch(-launch_vel_x, -160.f, 0.1f);
		}
	}
	// or Bounce falling platform
	else if (PlatformerFallingPlatform* falling_platform = dynamic_cast<PlatformerFallingPlatform*>(collided_entity))
	{
		falling_platform->launch_up(-(launch_vel_y / 4.f));
	}

	b_anim_paused = false;
	sprite_sheet.current_frame = 0;
	sprite_sheet.elapsed_time = sf::seconds(0.f);
}

void PlatformerTrampoline::update(float deltaTime)
{
	Entity::update(deltaTime);
}