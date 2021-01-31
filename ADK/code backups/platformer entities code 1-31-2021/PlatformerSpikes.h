#pragma once
#include "PlatformerDamageBox.h"

class PlatformerSpikes : public PlatformerDamageBox
{

public:
	PlatformerSpikes();

	virtual void init_collider() override;

	virtual void set_rotation(float newRot, bool bAffectCollider = false) override;

};

