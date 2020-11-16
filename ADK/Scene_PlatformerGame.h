#pragma once
#include "Engine/Scene.h"

class ADKCamera;
class PlatformerPlayer;

class Scene_PlatformerGame : public Scene
{

public:
	Scene_PlatformerGame();

// Scene overrides
	virtual void begin_scene(sf::RenderWindow& window) override;

	virtual void end_scene(sf::RenderWindow& window) override;

	virtual void process_events(sf::Event& event) override;

	virtual void update(float deltaTime) override;

	virtual void update_post(float deltaTime) override;

	virtual void render_pre(sf::RenderWindow& window) override;

	virtual void render(sf::RenderWindow& window) override;

// Scene_PlatformerGame specific
	void on_player_death();

	void on_end_freezeframe();

private:
	ADKCamera* camera;
	PlatformerPlayer* player;

	EntityList death_effects;

	sf::Font font;
	sf::Text player_debug_text;

};

