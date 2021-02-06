#pragma once
#include "PlatformerPlayer.h"

class GrapplehookPlayer : public PlatformerPlayer
{

public:

	GrapplehookPlayer();

private:

	sf::Vector2f hook_pos;		// world position of hook "tip". This is the pivot for our grapple hook.





};

