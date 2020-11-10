#include <SFML/Graphics.hpp>
#include <fstream>
#include <imgui.h>
#include <imgui-SFML.h>

#include "json.hpp"
#include "MoreColors.h"
#include "Scene_Editor.h"
#include "../Scene_PlatformerGame.h"
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

private:

	// Process user inputs
	void process_events();

	// Update game logic
	void update(float deltaTime);

	// Render game to screen
	void render();

	////////////////////////////////////////////////////

	template <class Scene>
	void switch_active_scene(bool b_end_scene = false);

	void show_engine_imgui();

private:
	// Struct holding engine configurations and settings
	FEngineConfig engine_config;

	// Represents the game window
	sf::RenderWindow window;

	// Represents the active scene showing to the player
	Scene* active_scene;

};

Engine::Engine()
	: active_scene(nullptr)
{
	//FEngineConfig default_engine;
	//nlohmann::json item;
	//item["Core"]["TicksPerSecond"] = default_engine.ticks_per_second;
	//item["Core"]["CapFpsToTick"] = default_engine.b_cap_framerate_to_ticks;
	//item["Graphics"]["bVsyncOn"] = default_engine.b_vsync_enabled;
	//item["Graphics"]["WinW"] = default_engine.default_window_width;
	//item["Graphics"]["WinH"] = default_engine.default_window_height;
	//item["Graphics"]["bCanResize"] = default_engine.b_can_resize;
	//item["Graphics"]["bFullscreen"] = engine_config.b_fullscreen;
	//std::ofstream config_stream_engine;
	//config_stream_engine.open("Saved/Config/Engine.json");
	//config_stream_engine << item.dump(4);
	//config_stream_engine.close();

	// Load Engine configs
	std::ifstream config_load("Saved/Config/Engine.json");
	nlohmann::json config_json;
	config_load >> config_json;
	engine_config.ticks_per_second = config_json["Core"]["TicksPerSecond"];
	engine_config.b_cap_framerate_to_ticks = config_json["Core"]["CapFpsToTick"];
	engine_config.b_vsync_enabled = config_json["Graphics"]["bVsyncOn"];
	engine_config.default_window_width = config_json["Graphics"]["WinW"];
	engine_config.default_window_height = config_json["Graphics"]["WinH"];
	engine_config.b_can_resize = config_json["Graphics"]["bCanResize"];
	engine_config.b_fullscreen = config_json["Graphics"]["bFullscreen"];

	// Create window
	window.create(sf::VideoMode(engine_config.default_window_width, engine_config.default_window_height),
		"ADK Engine",
		(engine_config.b_fullscreen ? sf::Style::Fullscreen : (engine_config.b_can_resize ? sf::Style::Default : sf::Style::Close)));
	window.setVerticalSyncEnabled(engine_config.b_vsync_enabled);
}

void Engine::run()
{
	if (sf::Joystick::isConnected(1))
	{
		printf("Joystick is connected.\n");
		printf("Joystick has %d.\n", sf::Joystick::getButtonCount(1));
	}

	// Initialize ImGui::SFML process
	ImGui::SFML::Init(window);

	// Initialize framerate and update times
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	sf::Time time_per_frame = sf::seconds(1.f / engine_config.ticks_per_second);

	// Choose the scene
	switch_active_scene<Scene_PlatformerGame>();

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

	// Shut down ImGui::SFML process
	ImGui::SFML::Shutdown();
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

template <class Scene>
void Engine::switch_active_scene(bool b_end_scene /*= false*/)
{
	if (b_end_scene && active_scene != nullptr)
	{
		active_scene->end_scene(window);
		delete active_scene;
	}

	active_scene = new Scene;
	active_scene->begin_scene(window);
}


void Engine::show_engine_imgui()
{
	ImGui::Begin("ADK Control Panel");

	// Switch Scenes
	if (ImGui::Button("Scene_PlatformerGame") && typeid(active_scene) != typeid(Scene_PlatformerGame))
	{
		switch_active_scene<Scene_PlatformerGame>(true);
	}
	if (ImGui::Button("Scene_Editor") && typeid(active_scene) != typeid(Scene_Editor))
	{
		switch_active_scene<Scene_Editor>(true);
	}
	ImGui::Separator();

	// Switch levels

	ImGui::End();
}

void Engine::update(float deltaTime)
{
	// ImGui::SFML Update
	ImGui::SFML::Update(window, sf::seconds(deltaTime));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tilde))
	{
		show_engine_imgui();
	}

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

	// Render ImGui::SFML. All creation of ImGui widgets must happen between ImGui::SFML::Update and ImGui::SFML::Render.
	ImGui::SFML::Render(window);

	window.display();
}

// --- MAIN ---
int main()
{
	Engine game;
	game.run();

	return 0;
}