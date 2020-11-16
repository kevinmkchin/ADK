#pragma once
#include "PlatformerMovingPlatform.h"

class PlatformerDamageBlock : public PlatformerMovingPlatform
{

public:
	PlatformerDamageBlock();

	virtual void init_collider() override;

	virtual void update(float deltaTime) override;

	virtual void collided(Entity* collided_entity) override;

};

