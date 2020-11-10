#pragma once
#include "PlatformerActivePlatform.h"

class PlatformerSpikes : public PlatformerActivePlatform
{

public:
	PlatformerSpikes();

	virtual void init_collider() override;

	virtual void set_rotation(float newRot, bool bAffectCollider = false) override;

	virtual void collided(Entity* collided_entity) override;

};

