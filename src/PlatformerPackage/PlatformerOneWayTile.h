#pragma once
#include "PlatformerTile.h"
class PlatformerOneWayTile : public PlatformerTile
{

public:
	PlatformerOneWayTile();

	virtual void init_collider() override;

};

