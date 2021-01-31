#pragma once
#include "PlatformerFallingPlatform.h"
class PlatformerFallingPlatformMedium : public PlatformerFallingPlatform
{
public:
	PlatformerFallingPlatformMedium();

	virtual void init_collider() override;

	virtual void launch_up(float yvel_up) override;

};

