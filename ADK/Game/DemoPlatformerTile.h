#pragma once
#include "../Engine/Entity.h"
class DemoPlatformerTile : public Entity
{

public:
	DemoPlatformerTile();

	virtual void InitCollider() override;

};

