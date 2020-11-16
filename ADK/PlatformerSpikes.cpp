#include "PlatformerSpikes.h"

PlatformerSpikes::PlatformerSpikes()
{
	set_active(false);

	texture_path = "Game/sheet2.png";
	set_frame_size(8, 8);
	set_animation_start_frame(0, 5);
	sprite_sheet.animations[0].anim_duration = sf::seconds(0.f);
	sprite_sheet.animations[0].num_frames = 1;

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