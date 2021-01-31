#include "PlatformerFallingPlatformMedium.h"

ADKOBJECT(PlatformerFallingPlatformMedium)
PlatformerFallingPlatformMedium::PlatformerFallingPlatformMedium()
{
	ADKOBJECT_BEGIN(PlatformerFallingPlatformMedium)
	ADKOBJECT_END()

	initial_delay_seconds = 0.4f;
	fall_acceleration = 80.f;
	launch_up_mult = 1.3f;

	texture_path = "Game/fallingplatform_a_16x16.png";
	set_frame_size(16, 16);
	set_animation_start_frame(0, 0);

	init_collider();
}

void PlatformerFallingPlatformMedium::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 16.f, 16.f);
}