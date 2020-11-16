#include "PlatformerDamageBlock.h"
#include "PlatformerPlayer.h"

PlatformerDamageBlock::PlatformerDamageBlock()
{
	direction = 0.f;
	distance_to_travel = 50.f;
	travel_time_in_seconds = 0.4f;
	pause_at_turn_in_seconds = 1.f;
	b_going_towards_end = false;
	travel_timer = 0.f;

	texture_path = "Game/blackhole2.png";
	set_frame_size(16, 16);
	set_animation_start_frame(0, 0);
	sprite_sheet.animations[0].anim_duration = sf::seconds(0.4f);
	sprite_sheet.animations[0].num_frames = 8;

	init_collider();
}

void PlatformerDamageBlock::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 2.f, 2.f, 12.f, 12.f);
}

void PlatformerDamageBlock::update(float deltaTime)
{
	PlatformerMovingPlatform::update(deltaTime);

	b_anim_paused = false;
}

void PlatformerDamageBlock::collided(Entity* collided_entity)
{
	if (PlatformerPlayer* player = dynamic_cast<PlatformerPlayer*>(collided_entity))
	{
		player->affect_health(-1.f);
	}
}