#include "DemoPlatformerOneWayPlatform.h"

DemoPlatformerOneWayPlatform::DemoPlatformerOneWayPlatform()
{
	load_texture_in_constructor("minimal_industry/black_tileset_b.png");

	init_collider();
}

void DemoPlatformerOneWayPlatform::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 8.f, 3.f);
}