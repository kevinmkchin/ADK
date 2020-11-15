#pragma once
#include "Engine/Scene.h"

class ADKCamera;
class PlatformerPlayer;

class Scene_PlatformerGame : public Scene
{

public:
	Scene_PlatformerGame();

	virtual void begin_scene(sf::RenderWindow& window) override;

	virtual void end_scene(sf::RenderWindow& window) override;

	virtual void process_events(sf::Event& event) override;

	virtual void update(float deltaTime) override;

	virtual void update_post(float deltaTime) override;

	virtual void render_pre(sf::RenderWindow& window) override;

	virtual void render(sf::RenderWindow& window) override;

private:
	ADKCamera* camera;
	PlatformerPlayer* player;

	sf::Font font;
	sf::Text player_debug_text;

};

