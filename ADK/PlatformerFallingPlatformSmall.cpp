#include "PlatformerFallingPlatformSmall.h"

ADKOBJECT(PlatformerFallingPlatformSmall)
PlatformerFallingPlatformSmall::PlatformerFallingPlatformSmall()
{
	ADKOBJECT_BEGIN(PlatformerFallingPlatformSmall)
	ADKOBJECT_END()

	initial_delay_seconds = 0.2f;
	fall_acceleration = 120.f;

	launch_up_mult = 1.6f;

	texture_path = "Game/fallingplatform_a_8x8.png";
	set_frame_size(8, 8);
	set_animation_start_frame(0, 0);

	init_collider();
}

void PlatformerFallingPlatformSmall::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 8.f, 8.f);
}