#pragma once
#include "Engine/TileEntity.h"

class PlatformerPlayer;

/** Platform or tile that actively run games logic. Keeps a reference to the player entity in order to perform checks.
	In the future, this could be expanded to a list of entities so that the platform doesn't only react to the player. */
class PlatformerActivePlatform : public TileEntity
{

public:
	PlatformerActivePlatform();

	PlatformerPlayer* entity_to_collide;

};

