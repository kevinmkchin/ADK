#pragma once
#include "Engine/Scene.h"

class PlatformerPlayer;

class Scene_Game : public Scene
{

public:
	Scene_Game();

	virtual void begin_scene(sf::RenderWindow& window) override;

	virtual void update(float deltaTime) override;

	virtual void render_pre(sf::RenderWindow& window) override;

	virtual void render(sf::RenderWindow& window) override;

private:
	PlatformerPlayer* player;

	sf::Font font;
	sf::Text player_debug_text;

};

