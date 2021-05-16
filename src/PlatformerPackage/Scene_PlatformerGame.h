#pragma once
#include "../Engine/Scene.h"

class ADKCamera;
class PlatformerPlayer;

class Scene_PlatformerGame : public Scene
{

public:
	Scene_PlatformerGame();

// Scene overrides
	virtual void begin_scene(sf::RenderWindow& window) override;

	virtual void end_scene(sf::RenderWindow& window) override;

	virtual void update(float deltaTime) override;

	virtual void update_post(float deltaTime) override;

	virtual void render_pre(sf::RenderWindow& window) override;

	virtual void render(sf::RenderWindow& window) override;

	virtual void show_scene_debugui() override;

// Scene_PlatformerGame specific
	void on_player_death();

	void on_end_freezeframe();

protected:
	// Init everything that persists throughout levels
	void initialize_gamescene();

	// Init everything only relevant to this level. Called to parse entity list, create player, camera, etc.
	void initialize_level();

	// Clean up level, delete dynamically allocated entities, etc.
	void end_level();

	void switch_level(std::string level_path);

	void restart_level();

private:
	std::string active_level_path;

	ADKCamera* camera;
	PlatformerPlayer* player;
	EntityList death_effects;

	sf::Font font;
	sf::Text player_debug_text;

};

