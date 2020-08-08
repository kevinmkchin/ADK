#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include "MoreColors.h"
#include "Scene_Editor.h"

#include "../ADKEditorMetaRegistry.h"

// Register all types and their identifiers for use in editor and save/load
REGISTER_ENTITY_TYPES

struct FEngineConfig
{
	// How often game logic is ticked/updated
	uint16_t TicksPerSecond;
	// Whether to render only on game logic ticks (i.e. tie framerate to game logic?)
	bool bCapFramerateToTicks;
	// Whether to use V-Sync
	bool bVSyncEnabled;
	// Game window resolution
	uint16_t DefaultWindowWidth;
	uint16_t DefaultWindowHeight;
	// Default window background color
	sf::Color WindowBackgroundColor;

	// Defaults
	FEngineConfig();
};

FEngineConfig::FEngineConfig()
	: TicksPerSecond(60)
	, bCapFramerateToTicks(true)
	, bVSyncEnabled(false)
	, DefaultWindowWidth(1280)
	, DefaultWindowHeight(720)
	, WindowBackgroundColor(MC_OLIVE)
{
}

class Engine
{
public:
	Engine();

	// Start game process
	void Run();

private:

	// Process user inputs
	void ProcessEvents();

	// Update game logic
	void Update(float deltaTime);

	// Render game to screen
	void Render();

private:
	FEngineConfig EngineConfig;

	sf::RenderWindow window;

	Scene* ActiveScene;

};

Engine::Engine()
	: window(sf::VideoMode(EngineConfig.DefaultWindowWidth, EngineConfig.DefaultWindowHeight), "ADK Engine")
{
}

void Engine::Run()
{
	// Read DefaultGame.ini to set up EngineConfig
	window.setVerticalSyncEnabled(EngineConfig.bVSyncEnabled);

	// Initialize ImGui::SFML
	ImGui::SFML::Init(window);

	// Initialize framerate and update times
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / EngineConfig.TicksPerSecond);

	// Choose the scene
	ActiveScene = new Scene_Editor();
	ActiveScene->BeginScene(window);

	// Game process loop
	while (window.isOpen())
	{
		ProcessEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame) // Loop until timeSinceLastUpdate is below required timePerFrame
		{
			timeSinceLastUpdate -= timePerFrame;

			ProcessEvents();

			Update(timePerFrame.asSeconds());

			if (EngineConfig.bCapFramerateToTicks) { Render(); }
		}
		if (EngineConfig.bCapFramerateToTicks == false) { Render(); }
	}


	ImGui::SFML::Shutdown();
}

void Engine::ProcessEvents()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
		{
			window.close();
		}
	}
}

void Engine::Update(float deltaTime)
{
	ActiveScene->PreUpdate(deltaTime);
	ActiveScene->Update(deltaTime);
	ActiveScene->PostUpdate(deltaTime);

	// ImGui::SFML Update
	ImGui::SFML::Update(window, sf::seconds(deltaTime));
	ImGui::Begin("demo");
	ImGui::Button("Hello1");
	ImGui::End();

}

void Engine::Render()
{
	window.clear(EngineConfig.WindowBackgroundColor);

	ImGui::SFML::Render(window);
	ActiveScene->PreRender(window);
	ActiveScene->Render(window);
	ActiveScene->PostRender(window);


	window.display();
}

// --- MAIN ---
int main()
{

	Engine game;
	game.Run();

	return 0;
}