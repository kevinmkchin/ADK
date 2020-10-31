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
	uint16_t TicksPerSecond = 60;
	// Whether to render only on game logic ticks (i.e. tie framerate to game logic?)
	bool bCapFramerateToTicks = true;
	// Whether to use V-Sync
	bool bVSyncEnabled = false;
	// Game window resolution
	uint16_t DefaultWindowWidth = 1600;
	uint16_t DefaultWindowHeight = 900;
	// Whether we can resize window
	bool bCanResize = true;
	// Default window background color
	sf::Color WindowBackgroundColor = MC_CHARCOAL;
};

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
	// Struct holding engine configurations and settings
	FEngineConfig EngineConfig;

	// Represents the game window
	sf::RenderWindow window;

	// Represents the active scene showing to the player
	Scene* ActiveScene;

};

Engine::Engine()
	: window(sf::VideoMode(EngineConfig.DefaultWindowWidth, EngineConfig.DefaultWindowHeight), 
		"ADK Engine", 
		(EngineConfig.bCanResize ? sf::Style::Default : sf::Style::Close))
{
}

void Engine::Run()
{
	// TODO Read DefaultGame.ini to set up EngineConfig
	window.setVerticalSyncEnabled(EngineConfig.bVSyncEnabled);
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
		if (ActiveScene != nullptr)
		{
			// Process player inputs in case we don't do an update tick this frame
			ProcessEvents();

			// Fixed timestep
			timeSinceLastUpdate += clock.restart();
			while (timeSinceLastUpdate > timePerFrame) // Loop until timeSinceLastUpdate is below required timePerFrame
			{
				timeSinceLastUpdate -= timePerFrame;

				// PROCESS GAME EVENTS AND PLAYER INPUT
				ProcessEvents();

				// UPDATE GAME (deltaTime is in SECONDS) ***
				Update(timePerFrame.asSeconds());

				// RENDER GAME if framerate capped to game ticks
				if (EngineConfig.bCapFramerateToTicks) { Render(); }
			}
			// RENDER GAME if framerate NOT capped to game ticks
			if (EngineConfig.bCapFramerateToTicks == false) { Render(); }
		}
	}
}

void Engine::ProcessEvents()
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
		ActiveScene->ProcessEvents(event);
	}
}

void Engine::Update(float deltaTime)
{
	ActiveScene->PreUpdate(deltaTime);
	ActiveScene->Update(deltaTime);
	ActiveScene->PostUpdate(deltaTime);
}

void Engine::Render()
{
	window.clear(EngineConfig.WindowBackgroundColor);

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