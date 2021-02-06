#include "GrapplehookPlayer.h"


ADKOBJECT(GrapplehookPlayer)
GrapplehookPlayer::GrapplehookPlayer()
{
	ADKOBJECT_BEGIN(GrapplehookPlayer);
	ADKOBJECT_END();

	// Attributes
	max_health = 1.f;
	health = max_health;
	// X vel
	curr_xvel = 0.f;
	max_xvel = 480.f;
	max_xvel_inair = 480.f;
	max_xvel_decel = 6000.f;
	acc_ground_xvel = 4500.f;
	dec_ground_xvel = 6000.f;
	acc_air_xvel = 4320.f;
	dec_air_xvel = 1600.f;
	// Y vel
	curr_yvel = 0.f;
	gravity = 1200.f;
	extra_down_acc = 400.f;
	jump_vel = 500.f;
	max_pos_yvel_default = 400.f;
	max_pos_yvel = max_pos_yvel_default;
	max_pos_yvel_when_pressing_s = max_pos_yvel_default * 2.f;
	max_pos_yvel_decel = 1400.f;
	// Jumping
	b_jumping = false;
	b_pending_jump = false;
	b_jumpkey_down = false;
	jump_peak_yvel = 50.f;
	percent_yvel_on_jump_release = 0.5f;
	// Jump buffering
	jump_buffer_maxhold_seconds = 0.30f;
	jump_buffer_maxtap_seconds = 0.12f;
	jump_buffer_timer_seconds = 0.f;
	// Coyote time
	coyote_time_default_seconds = 0.07f;
	coyote_time_timer_seconds = coyote_time_default_seconds;
	b_input_paused = false;
	input_pause_timer_seconds = 0.f;
	// Launched
	b_launched = false;
	// death grace period
	death_grace_period_default = 1.f;
	death_grace_period_timer = death_grace_period_default;
	// States
	b_try_fall_from_oneway = false;
	b_jumped_from_ground_this_frame = false;
	b_landed_this_frame = false;
	b_intersecting_oneway_lastframe = false;

	set_frame_size(64, 64);

	collider = BoxCollider(get_position().x, get_position().y, 0.5f, 1.f, 63.f, 63.f);
}