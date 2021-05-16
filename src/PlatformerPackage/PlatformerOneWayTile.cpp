#include "PlatformerOneWayTile.h"

ADKOBJECT(PlatformerOneWayTile)
PlatformerOneWayTile::PlatformerOneWayTile()
{
	ADKOBJECT_BEGIN(PlatformerOneWayTile)
	ADKOBJECT_END()

	texture_path = "Misc/platformer-old/sheet2.png";

	set_animation_start_frame(0, 6);

	init_collider();
}

void PlatformerOneWayTile::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 8.f, 4.f);
}