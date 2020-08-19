#pragma once

#include "Engine/Scene.h"
#include "Engine/Entity.h"

class Scene_TestOne : public Scene
{

public:
	virtual void BeginScene(sf::RenderWindow& window) override;

	//virtual void EndScene(sf::RenderWindow& window) override;

	//virtual void ProcessEvents(sf::Event& event) override;

	//virtual void PreUpdate(float deltaTime) override;

	virtual void Update(float deltaTime) override;

	//virtual void PostUpdate(float deltaTime) override;

	//// Called before render (probably used to set view and stuff)
	//virtual void PreRender(sf::RenderWindow& window) override;

	//// Draw the entities in this scene
	//virtual void Render(sf::RenderWindow& window) override;

	//// Stuff to do after rendering
	//virtual void PostRender(sf::RenderWindow& window) override;

private:
	Entity* player;

};

