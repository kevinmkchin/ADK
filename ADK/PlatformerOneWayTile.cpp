#include "PlatformerOneWayTile.h"

PlatformerOneWayTile::PlatformerOneWayTile()
{
	texture_path = "Game/sheet2.png";

	set_animation_start_frame(0, 6);

	init_collider();
}

void PlatformerOneWayTile::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 8.f, 4.f);
}