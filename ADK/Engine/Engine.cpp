#include "Engine.h"
#include "json.hpp"
#include "ADKTextures.h"
#include "ADKTimer.h"

#include "Scene_Editor.h"
#include "../Scene_PlatformerGame.h"


#pragma region REGISTRATION
// Define static maps for reference counting and texture pointers of ADKTexture
DEFINE_ADK_TEXTURES
#pragma endregion

Engine::Engine()
	: active_scene(nullptr)
	, b_update_paused(false)
	, update_pause_timer(-1.f)
	, game_speed(1.f)
	, b_pending_close(false)
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

#pragma region ImGuiStyle
	ImGuiStyle * style = &ImGui::GetStyle();
	//style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f;
	//style->FramePadding = ImVec2(5, 5);
	//style->FrameRounding = 4.0f;
	//style->ItemSpacing = ImVec2(12, 8);
	//style->ItemInnerSpacing = ImVec2(8, 6);
	//style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 17.0f;
	style->ScrollbarRounding = 9.0f;
	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4((float)23 / (float)255, (float)29 / (float)255, (float)34 / (float)255, 1.00f);//ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.467f, 0.533f, 0.6f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.467f, 0.6f, 0.6f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.251f, 0.267f, 1.00f);
#pragma endregion

	// Initialize framerate and update times
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	sf::Time time_per_frame = sf::seconds(1.f / engine_config.ticks_per_second);

	// Choose the scene
	switch_active_scene<Scene_Editor>();

	// Init core utils
	ADKTimer* adk_timer = ADKTimer::get_timer();

	// Game process loop
	while (window.isOpen())
	{
		if (active_scene != nullptr)
		{
			// Process player inputs in case we don't do an update tick this frame
			process_events();

			if (b_pending_close)
			{
				break;
			}

			// Fixed timestep
			time_since_last_update += clock.restart();
			while (time_since_last_update > time_per_frame) // Loop until time_since_last_update is below required time_per_frame
			{
				time_since_last_update -= time_per_frame;

				// PROCESS GAME EVENTS
				process_events();

				// UPDATE TIMER
				adk_timer->update_timer(time_per_frame.asSeconds());

				// UPDATE GAME (deltaTime is in SECONDS) ***
				update(time_per_frame.asSeconds());

				// RENDER GAME if framerate capped to game ticks
				if (engine_config.b_cap_framerate_to_ticks) { render(); }
			}
			// RENDER GAME if framerate NOT capped to game ticks
			if (engine_config.b_cap_framerate_to_ticks == false) { render(); }
		}
	}

	// Clean up core utils
	delete adk_timer;

	// Shut down ImGui::SFML process
	ImGui::SFML::Shutdown();
}

void Engine::cleanup()
{
	std::map<std::string, ADKClassDescription*> adkcdb = ADKClassDatabase::get_database()->get_all_class_descriptions();
	for (auto& it : adkcdb)
	{
		delete(it.second);
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
			if (active_scene)
			{ 
				active_scene->end_scene(window);
			}
			b_pending_close = true;
			window.close();
		}

		/*
		f1: pause update
		*/
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F1)
		{
			b_update_paused = !b_update_paused;
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
	active_scene->set_engine_instance(this);
	active_scene->begin_scene(window);
}

void Engine::show_engine_imgui()
{
	ImGui::Begin("ADK Control Panel");

	// Switch Scenes
	ImGui::Text("Switch Scene");
	if (ImGui::Button("Scene_PlatformerGame") && typeid(active_scene) != typeid(Scene_PlatformerGame))
	{
		switch_active_scene<Scene_PlatformerGame>(true);
	}
	if (ImGui::Button("Scene_Editor") && typeid(active_scene) != typeid(Scene_Editor))
	{
		switch_active_scene<Scene_Editor>(true);
	}
	ImGui::Separator();

	// Engine Control
	ImGui::Text("Engine Control");
	if (ImGui::Button("Pause Update"))
	{
		b_update_paused = !b_update_paused;
	}
	if (ImGui::Button("1x Speed"))
	{
		game_speed = 1.f;
	}
	ImGui::SameLine();
	if (ImGui::Button("0.5x Speed"))
	{
		game_speed = 0.5f;
	}
	ImGui::SameLine();
	if (ImGui::Button("0.2x Speed"))
	{
		game_speed = 0.2f;
	}
	ImGui::SameLine();
	if (ImGui::Button("2x Speed"))
	{
		game_speed = 2.f;
	}
	ImGui::Separator();

	// Scene specific ImGui
	active_scene->show_scene_debugui();
	ImGui::Separator();

	ImGui::End();
}

void Engine::pause_update(bool b_pause, float in_pause_in_seconds /*= -1.f*/)
{
	if (b_pause)
	{
		if (in_pause_in_seconds < 0.f)
		{
			b_update_paused = true;
			update_pause_timer = -1.f;
		}
		else
		{
			b_update_paused = true;
			update_pause_timer = in_pause_in_seconds;
		}
	}
	else
	{
		b_update_paused = false;
		if (active_scene)
		{
			active_scene->on_unpause();
		}
	}
}

void Engine::update(float deltaTime)
{
	// ImGui::SFML Update
	ImGui::SFML::Update(window, sf::seconds(deltaTime));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tilde))
	{
		show_engine_imgui();
	}

	// update pause timer
	if (update_pause_timer > 0.f)
	{
		update_pause_timer -= deltaTime;
		if (update_pause_timer <= 0.f)
		{
			b_update_paused = false;
			if (active_scene)
			{
				active_scene->on_unpause();
			}
		}
	}

	// check if update is paused
	if (b_update_paused == false)
	{
		// modify delta time with game speed
		deltaTime *= game_speed;
		// actually update
		active_scene->update_pre(deltaTime);
		active_scene->update(deltaTime);
		active_scene->update_post(deltaTime);
	}
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
	/* initialize random seed */
	srand(static_cast<unsigned int>(time(nullptr)));

	Engine game;
	game.run();
	game.cleanup();

	return 0;
}