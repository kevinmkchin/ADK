#pragma once
#include "PlatformerMovingPlatform.h"
class PlatformerTrampoline : public PlatformerActivePlatform
{

public:
	PlatformerTrampoline();

	virtual void init_collider() override;

	virtual void set_rotation(float newRot, bool bAffectCollider = false) override;

	virtual void collided(Entity* collided_entity) override;

	virtual void update(float deltaTime) override;

protected:

	float launch_vel_y;
	float launch_vel_x;

};

