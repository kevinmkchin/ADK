#pragma once
#include "Engine/Entity.h"

class TopDownPlayer : public Entity
{

public:
	TopDownPlayer();

	// --- UPDATE ---
	// Do game logic here, but do not render here. Not called if the Entity is not Active. Handles animation logic.
	virtual void Update(float deltaTime) override;

};

