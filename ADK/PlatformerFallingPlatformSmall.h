#pragma once
#include "PlatformerFallingPlatform.h"
class PlatformerFallingPlatformSmall : public PlatformerFallingPlatform
{

public:
	PlatformerFallingPlatformSmall();

	virtual void init_collider() override;

};

