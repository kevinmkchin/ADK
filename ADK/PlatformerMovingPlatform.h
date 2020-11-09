#pragma once
#include "Engine/TileEntity.h"

class PlatformerPlayer;

class PlatformerMovingPlatform : public TileEntity
{

public:
	PlatformerMovingPlatform();

	virtual void init_collider() override;

	virtual void update(float deltaTime) override;

protected:

	virtual void begin_play() override;

public:
	PlatformerPlayer* entity_to_collide;

protected:

	// direction to travel initially
	float direction;
	// distance to travel
	float distance_to_travel;
	// how long it should take to travel one direction fully
	float travel_time_in_seconds;
	// time to pause for at the end of travel
	float pause_at_turn_in_seconds;


	bool b_going_towards_end;
	float travel_timer;
	sf::Vector2f velocity_per_sec;
};

