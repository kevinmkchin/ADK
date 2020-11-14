#include "PlatformerTrampoline.h"
#include "PlatformerPlayer.h"

PlatformerTrampoline::PlatformerTrampoline()
	: launch_vel_y(300.f)
	, launch_vel_x(490.f)
{
	texture_path = "Game/trampoline.png";

	set_frame_size(16, 16);

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
}

void PlatformerTrampoline::update(float deltaTime)
{
	Entity::update(deltaTime);
}