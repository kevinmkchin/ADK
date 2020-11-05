#include <SFML/Graphics.hpp>

#include "MoreColors.h"
#include "Scene_Editor.h"
#include "../Game/Scene_Game.h"
#include "../ADKEditorMetaRegistry.h"
#include "ADKTextures.h"


#pragma region REGISTRATION
// Register all types and their identifiers for use in editor and save/load
REGISTER_ENTITY_TYPES
// Define static maps for reference counting and texture pointers of ADKTexture
DEFINE_ADK_TEXTURES
#pragma endregion

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
	// Default window background color
	sf::Color window_background_color = MC_CHARCOAL;
};

class Engine
{
public:
	Engine();

	// Start game process
	void run();

private:

	// Process user inputs
	void process_events();

	// Update game logic
	void update(float deltaTime);

	// Render game to screen
	void render();

private:
	// Struct holding engine configurations and settings
	FEngineConfig engine_config;

	// Represents the game window
	sf::RenderWindow window;

	// Represents the active scene showing to the player
	Scene* active_scene;

};

Engine::Engine()
	: window(sf::VideoMode(engine_config.default_window_width, engine_config.default_window_height), 
		"ADK Engine", 
		(engine_config.b_can_resize ? sf::Style::Default : sf::Style::Close))
{
}

void Engine::run()
{
	// TODO Read DefaultGame.ini to set up EngineConfig
	window.setVerticalSyncEnabled(engine_config.b_vsync_enabled);
	// Initialize framerate and update times
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	sf::Time time_per_frame = sf::seconds(1.f / engine_config.ticks_per_second);

	// Choose the scene
	active_scene = new Scene_Editor();
	active_scene->begin_scene(window);

	// Game process loop
	while (window.isOpen())
	{
		if (active_scene != nullptr)
		{
			// Process player inputs in case we don't do an update tick this frame
			process_events();

			// Fixed timestep
			time_since_last_update += clock.restart();
			while (time_since_last_update > time_per_frame) // Loop until time_since_last_update is below required time_per_frame
			{
				time_since_last_update -= time_per_frame;

				// PROCESS GAME EVENTS
				process_events();

				// UPDATE GAME (deltaTime is in SECONDS) ***
				update(time_per_frame.asSeconds());

				// RENDER GAME if framerate capped to game ticks
				if (engine_config.b_cap_framerate_to_ticks) { render(); }
			}
			// RENDER GAME if framerate NOT capped to game ticks
			if (engine_config.b_cap_framerate_to_ticks == false) { render(); }
		}
	}
}

void Engine::process_events()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		// Check if game closed (i.e. alt + f4 or close button)
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}

		// Process events for the ActiveScene
		active_scene->process_events(event);
	}
}

void Engine::update(float deltaTime)
{
	active_scene->update_pre(deltaTime);
	active_scene->update(deltaTime);
	active_scene->update_post(deltaTime);
}

void Engine::render()
{
	window.clear(engine_config.window_background_color);

	active_scene->render_pre(window);
	active_scene->render(window);
	active_scene->render_post(window);

	window.display();
}

// --- MAIN ---
int main()
{
	Engine game;
	game.run();

	return 0;
}