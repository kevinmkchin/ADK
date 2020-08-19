#pragma once
#include "Engine/Entity.h"

class TopDownPlayer : public Entity
{

public:
	// --- UPDATE ---
	// Do game logic here, but do not render here. Not called if the Entity is not Active. Handles animation logic.
	virtual void Update(float deltaTime);

	// --- RENDER ---
	// Draw the Entity here. Not called if the Entity is not Visible
	virtual void Render(sf::RenderTarget& target);

};

