#pragma once
#include "Engine/TileEntity.h"
class PlatformerTile : public TileEntity
{

public:
	PlatformerTile();

	virtual void init_collider() override;


};

