#pragma once

#include "Engine/Scene.h"
#include "Engine/Entity.h"

class Scene_TestOne : public Scene
{

public:
	virtual void BeginScene(sf::RenderWindow& window) override;

	virtual void Update(float deltaTime) override;

private:
	Entity* player;

	Entity* created;
};

