#include "DemoPlatformerTile.h"

DemoPlatformerTile::DemoPlatformerTile()
{
	load_texture_in_constructor("minimal_industry/black_tileset_a.png");

	set_frame_size(8, 8);

	init_collider();
}

void DemoPlatformerTile::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 16.f, 16.f);
}
