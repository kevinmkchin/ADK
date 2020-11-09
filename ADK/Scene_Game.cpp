#include "Scene_Game.h"
#include "Engine/ADKSaveLoad.h"
#include "Engine/MoreColors.h"
#include <vector>
#include <cstdlib>
#include <time.h>

#include "PlatformerPlayer.h"
#include "PlatformerSpikes.h"
#include "PlatformerMovingPlatform.h"

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

	player = new PlatformerPlayer;
	player->set_position(100.f, 200.f);
	player->collidable_platforms = new EntityList;
	player->damage_blocks = new EntityList;
	for (int i = 0; i < entities.size(); ++i)
	{
		// Add spikes to separate list
		if (PlatformerSpikes* spike = dynamic_cast<PlatformerSpikes*>(entities.at(i)))
		{
			player->damage_blocks->add(entities.at(i));
			continue;
		}

		// Give player reference to moving platforms
		if (PlatformerMovingPlatform* moving_platform = dynamic_cast<PlatformerMovingPlatform*>(entities.at(i)))
		{
			moving_platform->entity_to_collide = player;
		}

		player->collidable_platforms->add(entities.at(i));
	}
	entities.add(player);


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
