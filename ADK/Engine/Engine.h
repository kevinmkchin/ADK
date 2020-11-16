#pragma once

#include <SFML/Graphics.hpp>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include "MoreColors.h"

class Scene;

struct FEngineConfig
{
	// How often game logic is ticked/updated
	uint16_t ticks_per_second = 60;
	// Whether to render only on game logic ticks (i.e. tie framerate to game logic?)
	bool b_cap_framerate_to_ticks = true;
	// Whether to use V-Sync
	bool b_vsync_enabled = false;
	// Game window resolution
	uint16_t default_window_width = 1600;
	uint16_t default_window_height = 900;
	// Whether we can resize window
	bool b_can_resize = true;
	// Fullscreen
	bool b_fullscreen = false;
	// Default window background color
	sf::Color window_background_color = MC_CORNFLOWERBLUE;
};

class Engine
{
public:
	Engine();

	// Start game process
	void run();

	////////////////////////////////////////////////////

	// Call to pause or unpause update. pause_in_seconds is how long to pause update for before resuming.
	void pause_update(bool b_pause, float in_pause_in_seconds = -1.f);
	bool is_update_paused() const { return b_update_paused; }

	// Switch the active scene. b_end_scene calls Scene::end_scene if true.
	template <class Scene>
	void switch_active_scene(bool b_end_scene = false);

private:

	// Process user inputs
	void process_events();

	// Update game logic
	void update(float deltaTime);

	// Render game to screen
	void render();

	////////////////////////////////////////////////////

	void show_engine_imgui();

private:
	// Struct holding engine configurations and settings
	FEngineConfig engine_config;

	// Represents the game window
	sf::RenderWindow window;

	// Represents the active scene showing to the player
	Scene* active_scene;

	// Whether to pause updating the game
	bool b_update_paused;

	float update_pause_timer;

	// Speed at which to run the game. We multiply the delta time by this value before passing the delta time into scene update.
	float game_speed;

};