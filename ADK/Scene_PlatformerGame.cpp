#include <vector>
#include <cstdlib>
#include <time.h>
#include "Scene_PlatformerGame.h"
#include "Engine/ADKSaveLoad.h"
#include "Engine/MoreColors.h"
#include "Engine/ADKCamera.h"

#include "PlatformerPlayer.h"
#include "PlatformerSpikes.h"
#include "PlatformerActivePlatform.h"

Scene_PlatformerGame::Scene_PlatformerGame()
	: camera(nullptr)
	, player(nullptr)
{
	view_config.size_x = 320.f;
	view_config.size_y = 180.f;
	view_config.center_x = 160.f;
	view_config.center_y = 90.f;
}

void Scene_PlatformerGame::begin_scene(sf::RenderWindow& window)
{
	Scene::begin_scene(window);

	ADKSaveLoad Loader;
	Loader.load_to_scene("gym_platforms", *this);

	// Init Player
	player = new PlatformerPlayer;
	player->set_position(50.f, 5.f);
	player->collidable_platforms = new EntityList;
	player->damage_blocks = new EntityList;
	entities.add(player);

	// Init Camera
	camera = new ADKCamera(window);
	camera->set_follow_target(player);
	camera->set_follow_x(true);
	camera->set_follow_y(false);
	player->camera = camera;

	// Deal with entities on begin scene
	for (int i = 0; i < entities.size(); ++i)
	{
		// Add spikes to separate list
		if (PlatformerSpikes* spike = dynamic_cast<PlatformerSpikes*>(entities.at(i)))
		{
			player->damage_blocks->add(entities.at(i));
			continue;
		}

		// Give player reference to active platforms
		if (PlatformerActivePlatform* moving_platform = dynamic_cast<PlatformerActivePlatform*>(entities.at(i)))
		{
			moving_platform->entity_to_collide = player;
		}

		// Add entities to collidable platforms
		player->collidable_platforms->add(entities.at(i));
	}


	font.loadFromFile("Assets/Fonts/arial.ttf");
	player_debug_text.setFont(font);
	player_debug_text.setCharacterSize(13);
}

void Scene_PlatformerGame::end_scene(sf::RenderWindow& window)
{
	// Deallocate entity memory
	for (int i = entities.size() - 1; i != -1; --i)
	{
		entities.remove_and_destroy(entities.at(i));
	}

	Scene::end_scene(window);
}

void Scene_PlatformerGame::process_events(sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
	{
		camera->shake_camera(10.f, 10.f, 0.7f, 1.f, true);
	}
}

void Scene_PlatformerGame::update(float deltaTime)
{
	entities.update(deltaTime);

	player_debug_text.setString("debug");
}

void Scene_PlatformerGame::update_post(float deltaTime)
{
	camera->update_camera(deltaTime);
}

void Scene_PlatformerGame::render_pre(sf::RenderWindow& window)
{
	window.clear(MC_SOFTRED);
}

void Scene_PlatformerGame::render(sf::RenderWindow& window)
{
	entities.render(window);

	window.draw(player_debug_text);

	//entities.render(window, true);
}