#include "ExampleEntity.h"
#include <iostream>

ExampleEntity::ExampleEntity()
{
	TexturePath = "platformer-sheet.png";
}

void ExampleEntity::InitCollider()
{
	collider.left = GetPosition().x;
	collider.top = GetPosition().y;
	collider.width = (float)SpriteSheet.FrameSize.x;
	collider.height = (float)SpriteSheet.FrameSize.y;
}
