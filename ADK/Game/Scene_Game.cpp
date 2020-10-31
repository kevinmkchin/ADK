#include "Scene_Game.h"
#include "../Engine/ADKSaveLoad.h"
#include "../Engine/MoreColors.h"
#include <vector>
#include <cstdlib>
#include <time.h>

#include "PlatformerPlayer.h"

Scene_Game::Scene_Game()
{
	ViewConfig.SizeX = 640.f;
	ViewConfig.SizeY = 360.f;
	ViewConfig.CenterX = 320.f;
	ViewConfig.CenterY = 180.f;
}

void Scene_Game::BeginScene(sf::RenderWindow& window)
{
	InitializeSceneView(window);

	ADKSaveLoad Loader;
	Loader.LoadToScene("gym_oneway", *this);

	Platforms = new EntityList();
	for (int i = 0; i < Entities.size(); ++i)
	{
		Platforms->add(Entities.at(i));
	}

	Player = new PlatformerPlayer();
	Player->SetPosition(100.f, 100.f);
	Entities.add(Player);

	Player->collidable_platforms = Platforms;
	font.loadFromFile("Assets/Fonts/arial.ttf");
	player_debug_text.setFont(font);

}

void Scene_Game::Update(float deltaTime)
{
	Entities.Update(deltaTime);

	player_debug_text.setString("debug");
}

void Scene_Game::Render(sf::RenderWindow& window)
{
	//Entities.Render(window);

	window.draw(player_debug_text);

	Entities.Render(window, true);
}

void Scene_Game::PreRender(sf::RenderWindow& window)
{
	window.clear(MC_SOFTRED);
}
