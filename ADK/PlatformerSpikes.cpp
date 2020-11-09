#include "PlatformerSpikes.h"
#include "PlatformerPlayer.h"

PlatformerSpikes::PlatformerSpikes()
{
	texture_path = "Game/sheet2.png";

	set_animation_start_frame(0, 5);

	init_collider();
}

void PlatformerSpikes::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 0.5f, 4.f, 7.f, 4.f);
}

void PlatformerSpikes::set_rotation(float newRot, bool bAffectCollider /*= false*/)
{
	sprite_sheet.sprite.setRotation(newRot);

	if (newRot == 0.f)
	{
		collider.offset_x = 0.5f;
		collider.offset_y = 4.f;
		collider.width = 7.f;
		collider.height = 4.f;
	}
	else if (newRot == 90.f)
	{
		collider.offset_x = 0.f;
		collider.offset_y = 0.f;
		collider.width = 4.f;
		collider.height = 8.f;
	}
	else if (newRot == 180.f)
	{
		collider.offset_x = 0.f;
		collider.offset_y = 0.f;
		collider.width = 8.f;
		collider.height = 4.f;
	}
	else if (newRot == 270.f)
	{
		collider.offset_x = 4.f;
		collider.offset_y = 0.f;
		collider.width = 4.f;
		collider.height = 8.f;
	}
}

void PlatformerSpikes::collided(Entity* collided_entity)
{
	if (PlatformerPlayer* player = dynamic_cast<PlatformerPlayer*>(collided_entity))
	{
		player->affect_health(-1.f);
	}
}
