#include "DemoPlatformerTile.h"

DemoPlatformerTile::DemoPlatformerTile()
{
	SetTexturePathAndLoad("minimal_industry/black_tileset_a.png");

	SpriteSheet.FrameSize.x = 16;
	SpriteSheet.FrameSize.y = 16;

	set_origin(8.f, 8.f);
	use_origin_for_position(false);
	InitCollider();
}

void DemoPlatformerTile::InitCollider()
{
	collider = BoxCollider(GetPosition().x, GetPosition().y, 16.f, 16.f);
}
