#include "Scene_Game.h"
#include "../Engine/ADKSaveLoad.h"
#include "../Engine/MoreColors.h"
#include <vector>
#include <cstdlib>
#include <time.h>

#include "PlatformerPlayer.h"

Scene_Game::Scene_Game()
{
	view_config.size_x = 640.f;
	view_config.size_y = 360.f;
	view_config.center_x = 320.f;
	view_config.center_y = 180.f;
}

void Scene_Game::begin_scene(sf::RenderWindow& window)
{
	initialize_scene_view(window);

	ADKSaveLoad Loader;
	Loader.load_to_scene("gym_oneway", *this);

	platforms = new EntityList();
	for (int i = 0; i < entities.size(); ++i)
	{
		platforms->add(entities.at(i));
	}

	player = new PlatformerPlayer();
	player->set_position(100.f, 100.f);
	entities.add(player);

	player->collidable_platforms = platforms;
	font.loadFromFile("Assets/Fonts/arial.ttf");
	player_debug_text.setFont(font);

}

void Scene_Game::update(float deltaTime)
{
	entities.update(deltaTime);

	player_debug_text.setString("debug");
}

void Scene_Game::render(sf::RenderWindow& window)
{
	entities.render(window);

	window.draw(player_debug_text);

	entities.render(window, true);
}

void Scene_Game::render_pre(sf::RenderWindow& window)
{
	window.clear(MC_SOFTRED);
}
