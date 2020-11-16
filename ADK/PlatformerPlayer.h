#pragma once
#include "Engine/Entity.h"

class EntityList;
class ADKCamera;
class Scene_PlatformerGame;

struct DustGroup
{
	std::vector<sf::RectangleShape*> dust_effects;

	float dust_life_timer = 0.f;

	float dust_tick_timer_default = 0.1f;
	float dust_tick_timer = dust_tick_timer_default;
};

class PlatformerPlayer : public Entity
{

// Overrides
public:
	PlatformerPlayer();

	virtual void update(float deltaTime) override;

	virtual void init_collider() override;

	virtual void render(sf::RenderTarget& target) override;

protected:
	virtual void begin_play() override;

// PlatformerPlayer specific
public:
	// platforms we can stand on
	EntityList* collidable_platforms;

	// entities we don't collide with but still trigger by collision (includes spikes and checkpoints)
	EntityList* trigger_boxes;

	void affect_health(float delta);

	void launch(float in_xvel, float in_yvel, float input_pause = 0.f);

	void restart_player();

	void die();

protected:

	void read_input(float dt);

	void resolve_movement(float dt);

	void update_dust_effects(float dt);

	// reset memory and states like whether we jumped this frame or not
	void reset_states();

public:
	ADKCamera* camera;

	Scene_PlatformerGame* owning_scene;

#pragma region ATTRIBUTES

protected:

	float max_health;

	float health;

#pragma endregion

#pragma region MOVEMENT

protected:
	// x
	float curr_xvel;		// current x velocity
	float max_xvel;			// maximum x velocity--current x velocity is will try to stay at this value--max move speed basically
	float max_xvel_inair;	// maximum x velocity in air
	float max_xvel_decel;	// deceleration in x velocity while exceeding max_xvel, should be greater than acceleration
	float acc_ground_xvel;	// x velocity acceleration on ground
	float dec_ground_xvel;	// x velocity deceleration on ground
	float acc_air_xvel;		// x velocity acceleration in air
	float dec_air_xvel;		// x velocity deceleration in air

	// y
	float curr_yvel;
	float gravity;
	float extra_down_acc;				// extra down acceleration applied when pressing S or down
	float jump_vel;						// initial velocity when jumping
	float max_pos_yvel_default;			// max fall speed default
	float max_pos_yvel;					// max fall speed
	float max_pos_yvel_when_pressing_s; // max fall speed while pressing s
	float max_pos_yvel_decel;			// deceleration in fall speed when we are trying to go back down to max_pos_yvel
	bool b_jumping;
	bool b_pending_jump;	// flag to indicate whether we are pending a jump
	bool b_jumpkey_down;	// flag to indicate whether we are currently holding down the jump key
	float jump_peak_yvel;	// while jumping, if yvel is between -jump_peak_yvel to +jump_peak_yvel, half gravity is applied
	float percent_yvel_on_jump_release;		// percent of current yvel to cut yvel to when we release the jump key (for variable jump height)

	// jump buffering
	float jump_buffer_maxhold_seconds;		// jump buffer (the time before landing you are allowed to HOLD jump and still jump on the frame you land)
	float jump_buffer_maxtap_seconds;		// jump buffer (the time before landing you are allowed to TAP jump and still jump on the frame you land)
	float jump_buffer_timer_seconds;		// timer to keep track of how much time it's been since you pressed jump

	// coyote time
	float coyote_time_default_seconds;		// coyote time to give the player
	float coyote_time_timer_seconds;		// timer to keep track of coyote time left

	bool b_input_paused;
	float input_pause_timer_seconds;

	// launched
	bool b_launched;						// whether we are in air because we've been launched

	// death grace
	float death_grace_period_default;
	float death_grace_period_timer;			// timer after dying during which we can't die again

	// states
	bool b_try_fall_from_oneway;
	bool b_jumped_from_ground_this_frame;	// whether we jumped off the ground this frame
	bool b_landed_this_frame;				// whether we landed on ground this frame
	bool b_launched_this_frame;				// whether or not we've been launched

	// state memory
	bool b_intersecting_oneway_lastframe;

#pragma endregion

#pragma region DUSTEFFECT

private:

	float dust_tick_interval_seconds;		// specifies tick interval for dusts
	int dust_dissipation_chance;			// chance to dissipate on dust tick. out of 100
	int dust_move_up_chance;				// chance to move up on dust tick. out of 100
	int dust_move_sideways_chance;			// chance to move sideways on dust tick. out of 100

	float dust_lifetime_jump;				// lifetime for dust that kicks up on jump
	int dust_num_pixels_min_jump;			// min num of dust pixels to create on jump
	int dust_num_pixels_max_jump;			// min num of dust pixels to create on jump
	int dust_down_adjustment_jump;			// adjust y position of dust spawn on jump
	int dust_width_min_jump;				// dust width min on jump
	int dust_width_variance_jump;			// width variance on jump
	int dust_height_min_jump;				// dust height min on jump
	int dust_height_variance_jump;			// height variance on jump

	float dust_lifetime_land;				// lifetime for dust that kicks up on land
	int dust_num_pixels_min_land;			// min num of dust pixels to create on land
	int dust_num_pixels_max_land;			// min num of dust pixels to create on land
	int dust_down_adjustment_land;			// adjust y position of dust spawn on land
	int dust_width_min_land;				// dust width min on land
	int dust_width_variance_land;			// width variance on land
	int dust_height_min_land;				// dust height min on land
	int dust_height_variance_land;			// height variance on land
	float dust_kickup_maxyvel_percent;		// percentage of max pos y velocity at which we were falling at before landing where we still kick up dust

	int dust_group_size;					// dust_group max size
	std::vector<DustGroup> dust_groups;

#pragma endregion

};