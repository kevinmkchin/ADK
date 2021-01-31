#include "PlatformerTile.h"

ADKOBJECT(PlatformerTile)
PlatformerTile::PlatformerTile()
{
	ADKOBJECT_BEGIN(PlatformerTile)
	ADKOBJECT_END()

	set_active(false);

	texture_path = "Game/sheet1.png";

	set_frame_size(8, 8);

	init_collider();
}

void PlatformerTile::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 8.f, 8.f);
}