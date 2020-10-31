#include "DemoPlatformerOneWayPlatform.h"

DemoPlatformerOneWayPlatform::DemoPlatformerOneWayPlatform()
{
	SetTexturePathAndLoad("minimal_industry/black_tileset_c.png");
	InitCollider();

	SpriteSheet.FrameSize.x = 16;
	SpriteSheet.FrameSize.y = 16;
}

void DemoPlatformerOneWayPlatform::InitCollider()
{
	collider = BoxCollider(GetPosition().x, GetPosition().y, 16.f, 6.f);
}