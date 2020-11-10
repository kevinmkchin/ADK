#pragma once
#include "PlatformerMovingPlatform.h"
class PlatformerFallingPlatform : public PlatformerActivePlatform
{

public:
	PlatformerFallingPlatform();

	virtual void init_collider() override;

	virtual void update(float deltaTime) override;

protected:
	virtual void update_falling_active();

protected:
	
	float initial_delay_seconds;	// initial delay before falling
	float fall_acceleration;		// acceleration while falling
	float stop_y;					// y at which to stop falling

	float initial_delay_timer;		// timer for initial delay
	float curr_fall_vel;			// holds current fall speed
	bool b_falling_active;			// holds whether this block has been activated

};

