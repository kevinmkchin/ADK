#pragma once
#include "Engine/TileEntity.h"
class DemoPlatformerTile : public TileEntity
{

public:
	DemoPlatformerTile();

	virtual void init_collider() override;


};

