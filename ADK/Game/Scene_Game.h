#pragma once
#include "../Engine/Scene.h"

class PlatformerPlayer;

class Scene_Game : public Scene
{

public:
	Scene_Game();

	virtual void BeginScene(sf::RenderWindow& window) override;

	virtual void Update(float deltaTime) override;

	virtual void PreRender(sf::RenderWindow& window) override;

	virtual void Render(sf::RenderWindow& window) override;

private:
	PlatformerPlayer* Player;

	EntityList* Platforms;

	sf::Font font;
	sf::Text player_debug_text;

};

