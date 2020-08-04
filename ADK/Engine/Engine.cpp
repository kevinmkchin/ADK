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
	void Update(float DeltaTime);

	// Render game to screen
	void Render();

private:
	const FEngineConfig EngineConfig;

	sf::RenderWindow Window;

};

Engine::Engine()
	: Window(sf::VideoMode(EngineConfig.DefaultWindowWidth, EngineConfig.DefaultWindowHeight), "ADK Engine")
{
}

void Engine::Run()
{
	// Set V-Sync
	Window.setVerticalSyncEnabled(EngineConfig.bVSyncEnabled);

	// Initialize framerate and update times
	sf::Clock Clock;
	sf::Time TimeSinceLastUpdate = sf::Time::Zero;
	sf::Time TimePerFrame = sf::seconds(1.f / EngineConfig.TicksPerSecond);

	// Game process loop
	while (Window.isOpen())
	{
		ProcessEvents();
		TimeSinceLastUpdate += Clock.restart();
		while (TimeSinceLastUpdate > TimePerFrame) // Loop until timeSinceLastUpdate is below required timePerFrame
		{
			TimeSinceLastUpdate -= TimePerFrame;

			ProcessEvents();
			
			Update(TimePerFrame.asSeconds());

			if (EngineConfig.bCapFramerateToTicks) { Render(); }
		}
		if (EngineConfig.bCapFramerateToTicks == false) { Render(); }
	}
}

void Engine::ProcessEvents()
{
	sf::Event Event;
	while (Window.pollEvent(Event))
	{
		if (Event.type == sf::Event::Closed)
		{
			Window.close();
		}
	}
}

void Engine::Update(float DeltaTime)
{

}

void Engine::Render()
{
	Window.clear();



	Window.display();
}

// --- MAIN ---
int main()
{
	Engine Game;
	Game.Run();

	return 0;
}