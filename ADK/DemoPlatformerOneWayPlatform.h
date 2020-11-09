#pragma once
#include "DemoPlatformerTile.h"
class DemoPlatformerOneWayPlatform : public DemoPlatformerTile
{

public:
	DemoPlatformerOneWayPlatform();

	virtual void init_collider() override;

};

