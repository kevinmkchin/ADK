#pragma once
#include "PlatformerTriggerBox.h"
class PlatformerSpawn : public PlatformerTriggerBox
{
public:
	PlatformerSpawn();

	virtual void init_collider() override;

	virtual void collided(Entity* collided_entity) override;

	int spawn_id;

	bool b_activated;
};

// Spawnpoint id comparator
struct SpawnIDComparator
{
	bool operator ()(const PlatformerSpawn* entity1, const PlatformerSpawn* entity2)
	{
		return entity1->spawn_id <= entity2->spawn_id;
	}
};