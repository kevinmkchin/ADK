#include "DemoPlatformerTile.h"

DemoPlatformerTile::DemoPlatformerTile()
{
	set_texture_path_and_load("minimal_industry/black_tileset_a.png");

	sprite_sheet.frame_size.x = 16;
	sprite_sheet.frame_size.y = 16;

	set_origin(8.f, 8.f);
	use_origin_for_position(false);
	init_collider();
}

void DemoPlatformerTile::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 16.f, 16.f);
}
