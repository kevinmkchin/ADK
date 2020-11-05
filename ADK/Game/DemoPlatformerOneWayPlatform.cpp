#include "DemoPlatformerOneWayPlatform.h"

DemoPlatformerOneWayPlatform::DemoPlatformerOneWayPlatform()
{
	set_texture_path_and_load("minimal_industry/black_tileset_c.png");
	init_collider();

	sprite_sheet.frame_size.x = 16;
	sprite_sheet.frame_size.y = 16;
}

void DemoPlatformerOneWayPlatform::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 16.f, 6.f);
}