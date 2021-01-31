#pragma once
#include "PlatformerTriggerBox.h"
class PlatformerDamageBox : public PlatformerTriggerBox
{

public:
	virtual void collided(Entity* collided_entity) override;

};

