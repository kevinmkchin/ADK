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

#pragma region MOVEMENT

	// x
	float curr_xvel;		// current x velocity
	float max_xvel;			// maximum x velocity--current x velocity is capped at this value--max move speed basically
	float max_xvel_inair;	// maximum x velocity in air
	float acc_ground_xvel;	// x velocity acceleration on ground
	float dec_ground_xvel;	// x velocity deceleration on ground
	float acc_air_xvel;		// x velocity acceleration in air
	float dec_air_xvel;		// x velocity deceleration in air

	// y
	float curr_yvel;
	float gravity;
	float extra_down_acc;	// extra down acceleration applied when pressing S or down
	float jump_vel;			// initial velocity when jumping
	float max_pos_yvel;		// max fall speed
	bool b_jumping;
	bool b_pending_jump;	// flag to indicate whether we are pending a jump
	bool b_jumpkey_down;	// flag to indicate whether we are currently holding down the jump key

	// jump buffering
	float jump_buffer_maxhold_seconds;		// jump buffer (the time before landing you are allowed to HOLD jump and still jump on the frame you land)
	float jump_buffer_maxtap_seconds;		// jump buffer (the time before landing you are allowed to TAP jump and still jump on the frame you land)
	float jump_buffer_timer_seconds;	// timer to keep track of how much time it's been since you pressed jump

	// coyote time
	float coyote_time_default_seconds;	// coyote time to give the player
	float coyote_time_timer_seconds;	// timer to keep track of coyote time left



#pragma endregion

};