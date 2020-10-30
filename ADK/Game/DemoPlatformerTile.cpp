#include "DemoPlatformerTile.h"

DemoPlatformerTile::DemoPlatformerTile()
{
	SetTexturePathAndLoad("minimal_industry/black_tileset_a.png");
	InitCollider();

	SpriteSheet.FrameSize.x = 16;
	SpriteSheet.FrameSize.y = 16;
}

void DemoPlatformerTile::InitCollider()
{
	collider = BoxCollider(GetPosition().x, GetPosition().y, 16.f, 16.f);
}
