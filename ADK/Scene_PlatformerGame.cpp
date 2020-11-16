#include <vector>
#include <cstdlib>
#include <time.h>
#include <experimental/filesystem>
#include "Scene_PlatformerGame.h"
#include "Engine/ADKSaveLoad.h"
#include "Engine/MoreColors.h"
#include "Engine/ADKCamera.h"
#include "Engine/ADKMath.h"
#include "Engine/ADKTimer.h"

#include "PlatformerPlayer.h"
#include "PlatformerTriggerBox.h"
#include "PlatformerActivePlatform.h"

Scene_PlatformerGame::Scene_PlatformerGame()
	: camera(nullptr)
	, player(nullptr)
	, death_effects(EntityList())
{
	view_config.size_x = 320.f;
	view_config.size_y = 180.f;
	view_config.center_x = 160.f;
	view_config.center_y = 90.f;
}

void Scene_PlatformerGame::begin_scene(sf::RenderWindow& window)
{
	Scene::begin_scene(window);

	initialize_gamescene();
	switch_level("gym_platforms");

	font.loadFromFile("Assets/Fonts/arial.ttf");
	player_debug_text.setFont(font);
	player_debug_text.setCharacterSize(13);
}

void Scene_PlatformerGame::end_scene(sf::RenderWindow& window)
{
	end_level();

	Scene::end_scene(window);
}

void Scene_PlatformerGame::update(float deltaTime)
{
	level_entities.update(deltaTime);
	death_effects.update(deltaTime); // TODO try removing this line

	player_debug_text.setString("debug");
}

void Scene_PlatformerGame::update_post(float deltaTime)
{
	if (camera)
	{
		camera->update_camera(deltaTime);
	}
}

void Scene_PlatformerGame::render_pre(sf::RenderWindow& window)
{
	window.clear(MC_CORNFLOWERBLUE);
}

void Scene_PlatformerGame::render(sf::RenderWindow& window)
{
	level_entities.render(window);
	death_effects.render(window);

	window.draw(player_debug_text);

	//entities.render(window, true);
}

void Scene_PlatformerGame::initialize_gamescene()
{

	// Init death effects
	death_effects.clear();
	Entity* de1 = new Entity();
	de1->set_active(false);
	de1->set_visible(false);
	de1->set_texture_path_and_load("Game/death/deatheffect1.png");
	de1->set_origin(de1->sprite_sheet.frame_size.x / 2.f, de1->sprite_sheet.frame_size.y / 2.f);
	death_effects.add(de1);
	Entity* de2 = new Entity();
	de2->set_active(false);
	de2->set_visible(false);
	de2->set_texture_path_and_load("Game/death/deatheffect2.png");
	de2->set_origin(de2->sprite_sheet.frame_size.x / 2.f, de2->sprite_sheet.frame_size.y / 2.f);
	death_effects.add(de2);
	Entity* de3 = new Entity();
	de3->set_active(false);
	de3->set_visible(false);
	de3->set_texture_path_and_load("Game/death/deatheffect3.png");
	de3->set_origin(de3->sprite_sheet.frame_size.x / 2.f, de3->sprite_sheet.frame_size.y / 2.f);
	death_effects.add(de3);
	Entity* de4 = new Entity();
	de4->set_active(false);
	de4->set_visible(false);
	de4->set_texture_path_and_load("Game/death/deatheffect4.png");
	de4->set_origin(de4->sprite_sheet.frame_size.x / 2.f, de4->sprite_sheet.frame_size.y / 2.f);
	death_effects.add(de4);
	Entity* de5 = new Entity();
	de5->set_active(false);
	de5->set_visible(false);
	de5->set_texture_path_and_load("Game/death/deatheffect5.png");
	de5->set_origin(de5->sprite_sheet.frame_size.x / 2.f, de5->sprite_sheet.frame_size.y / 2.f);
	death_effects.add(de5);
	Entity* de6 = new Entity();
	de6->set_active(false);
	de6->set_visible(false);
	de6->set_texture_path_and_load("Game/death/deatheffect6.png");
	de6->set_origin(de6->sprite_sheet.frame_size.x / 2.f, de6->sprite_sheet.frame_size.y / 2.f);
	death_effects.add(de6);

}

void Scene_PlatformerGame::initialize_level(sf::RenderWindow& window)
{
	// Init Player
	player = new PlatformerPlayer;
	player->set_position(50.f, 5.f);
	player->owning_scene = this;

	// Init Camera
	camera = new ADKCamera(window);
	camera->set_follow_target(player);
	camera->set_follow_x(true);
	camera->set_follow_y(false);
	player->camera = camera;

	// Add player to level entities list
	level_entities.add(player);

	// Parse level entities
	for (int i = 0; i < level_entities.size(); ++i)
	{
		// Add damage blocks to separate list
		if (PlatformerTriggerBox* damage_block = dynamic_cast<PlatformerTriggerBox*>(level_entities.at(i)))
		{
			player->trigger_boxes->add(level_entities.at(i));
			continue;
		}

		// Give player reference to active platforms
		if (PlatformerActivePlatform* moving_platform = dynamic_cast<PlatformerActivePlatform*>(level_entities.at(i)))
		{
			moving_platform->entity_to_collide = player;
		}

		// Add entities to collidable platforms
		// TODO ONLY IF COLLIDABLE DUDE
		player->collidable_platforms->add(level_entities.at(i));
	}
}

void Scene_PlatformerGame::end_level()
{
	if (camera != nullptr)
	{
		delete camera;
		camera = nullptr;
	}

	// Delete level entities. Any entities that should persist between level loads shouldn't be in this entitylist.
	for (int i = level_entities.size() - 1; i != -1; --i)
	{
		level_entities.remove_and_destroy(level_entities.at(i));
	}
}

void Scene_PlatformerGame::switch_level(std::string level_path)
{
	end_level(); // remove level entities from entities EntityList and clear level_entities

	ADKSaveLoad Loader;
	Loader.load_to_scene(level_path, *this); // load new level but don't empty entities

	initialize_level(*render_window_ptr);
}

void Scene_PlatformerGame::on_player_death()
{
	Entity* selected_deatheffect = death_effects.at(ADKMath::rand_int_range(0, death_effects.size() - 1));
	selected_deatheffect->set_position(player->get_position().x + (player->sprite_sheet.frame_size.x / 2.f), player->get_position().y + (player->sprite_sheet.frame_size.y / 2.f));
	selected_deatheffect->set_visible(true);

	pause_update(true);

	// This is how to call set a timed callback function in ADK
	ADKTimer::get_timer()->set_timed_callback(&Scene_PlatformerGame::on_end_freezeframe, this, 0.1f);
}

void Scene_PlatformerGame::on_end_freezeframe()
{
	pause_update(false);

	Entity* death_effect = nullptr;
	for (int i = 0; i < death_effects.size(); ++i)	// find index of selected death effect
	{
		if (death_effects.at(i)->is_visible())
		{
			death_effect = death_effects.at(i);
		}
	}
	death_effect->set_visible(false);

	if (camera)
	{
		camera->shake_camera(7.f, 7.f, 0.75f, 0.8f, false);
	}

	ADKTimer::get_timer()->set_timed_callback(&PlatformerPlayer::restart_player, player, 0.6f);
}

void Scene_PlatformerGame::show_scene_debugui()
{
	ImGui::Text("PlatformerGame: Load Level");

	std::string levels_directory = "Assets/Levels/";
	namespace stdfs = std::experimental::filesystem;
	std::vector<std::string> filenames;
	const stdfs::directory_iterator end{};
	for (stdfs::directory_iterator iter{ levels_directory }; iter != end; ++iter)
	{
		if (stdfs::is_regular_file(*iter))
		{
			filenames.push_back(iter->path().string());
		}
	}

	for (std::size_t i = 0; i < filenames.size(); ++i)
	{
		ImGui::PushID(i);
		std::string name = filenames.at(i);
		name = name.substr(levels_directory.length());
		if (ImGui::Button(name.c_str()))
		{
			switch_level(name);
		}
		ImGui::PopID();
	}
}
