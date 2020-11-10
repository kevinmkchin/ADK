#include "PlatformerTrampoline.h"
#include "PlatformerPlayer.h"

PlatformerTrampoline::PlatformerTrampoline()
	: launch_vel(-280.f)
{
	set_frame_size(8, 8);

	init_collider();
}

void PlatformerTrampoline::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 2.f, 6.f, 4.f, 2.f);
}

void PlatformerTrampoline::collided(Entity* collided_entity)
{
	if (PlatformerPlayer* player = dynamic_cast<PlatformerPlayer*>(collided_entity))
	{
		player->launch_up(launch_vel);
	}
}

void PlatformerTrampoline::update(float deltaTime)
{
	Entity::update(deltaTime);
}