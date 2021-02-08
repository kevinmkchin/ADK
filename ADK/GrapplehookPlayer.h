#pragma once
#include "PlatformerPlayer.h"

/*  This is essentially a modified copy of PlatformerPlayer because I've overriden and 
	modified most functions. */
class GrapplehookPlayer : public PlatformerPlayer
{

public:

	GrapplehookPlayer();

	virtual void update(float deltaTime) override;

	virtual void render(sf::RenderTarget& target) override;

protected:

	virtual void read_input(float dt) override; // also updates velocities etc

	virtual void resolve_movement(float dt) override;

	virtual void reset_states() override;

private:
	float max_xvel_inair_by_ad;			// max xvel possible in air through pressing A or D

	bool b_left_mouse_down;

	sf::Vector2f hook_start_offset;		// offset to player position for hook start
	float hook_max_len;					// max length grapple hook can reach before having to retract
	sf::Vector2f hook_pos;				// world position of hook "tip". This is the pivot for our grapple hook.
	sf::Vector2f hook_force;			// current force pulling us towards the hook represented as an acceleration because mass doesn't matter F=ma.
	bool b_hook_active;					// grapple hook on or off

	sf::RenderWindow* game_window;

};

