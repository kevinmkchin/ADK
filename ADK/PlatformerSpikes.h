#pragma once
#include "PlatformerDamageBlock.h"

class PlatformerSpikes : public PlatformerDamageBlock
{

public:
	PlatformerSpikes();

	virtual void init_collider() override;

	virtual void set_rotation(float newRot, bool bAffectCollider = false) override;

};

