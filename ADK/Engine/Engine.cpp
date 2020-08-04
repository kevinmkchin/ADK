#include <SFML/Graphics.hpp>

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


	// Defaults
	FEngineConfig();
};

FEngineConfig::FEngineConfig()
	: TicksPerSecond(60)
	, bCapFramerateToTicks(true)
	, bVSyncEnabled(false)
	, DefaultWindowWidth(1280)
	, DefaultWindowHeight(720)
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

};

Engine::Engine()
	: window(sf::VideoMode(EngineConfig.DefaultWindowWidth, EngineConfig.DefaultWindowHeight), "ADK Engine")
{
}

void Engine::Run()
{
	// Read DefaultGame.ini to set up EngineConfig

	// Set V-Sync
	window.setVerticalSyncEnabled(EngineConfig.bVSyncEnabled);

	// Initialize framerate and update times
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / EngineConfig.TicksPerSecond);

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
}

void Engine::ProcessEvents()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}
	}
}

void Engine::Update(float deltaTime)
{

}

void Engine::Render()
{
	window.clear();



	window.display();
}

// --- MAIN ---
int main()
{
	Engine game;
	game.Run();

	return 0;
}