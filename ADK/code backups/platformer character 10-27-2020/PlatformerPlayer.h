#pragma once
#include "../Engine/Entity.h"

class EntityList;

class PlatformerPlayer : public Entity
{

public:
	PlatformerPlayer();

	virtual void Update(float deltaTime) override;

	virtual void InitCollider() override;

	EntityList* collidable_platforms;

protected:
	void read_input(float dt);

	void resolve_movement(float dt);

	/** This function is a fall-back in case something goes wrong and we ended up colliding into something after moving.
		This function finds the overlap of least resistance and corrects the position of the player by that overlap. */
	void resolve_outstanding_collisions(float dt);

private:

	// Movement
	float curr_xvel;		// current x velocity
	float max_xvel;			// maximum x velocity--current x velocity is capped at this value
	float acc_ground_xvel;	// x velocity acceleration on ground
	float dec_ground_xvel;	// x velocity decceleration on ground
	float acc_air_xvel;		// x velocity acceleration in air
	float dec_air_xvel;		// x velocity decceleration in air

	float curr_yvel;
	float gravity;
	float extra_down_acc;	// extra down acceleration applied when pressing S or down
	float jump_vel;
	float max_pos_yvel;
	bool b_jumping;
	bool b_pending_jump;	// flag to indicate whether we are pending a jump
	bool b_jumpkey_pressed;	// flag to indicate whether we are currently holding the jumpkey
};