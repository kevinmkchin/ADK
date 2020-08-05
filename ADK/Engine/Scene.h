#pragma once

#include <SFML/Graphics.hpp>
#include "InternalUtilities/EntityList.h"
#include "InternalUtilities/AssetManager.h"

class Scene
{
public:
	Scene();

	void PreUpdate(float deltaTime);

	void Update(float deltaTime);

	void PostUpdate(float deltaTime);

	// Called before render (probably used to set view and stuff)
	void PreRender(sf::RenderTarget& target);

	// Draw the entities in this scene
	void Render(sf::RenderTarget& target);

	// Stuff to do after rendering
	void PostRender(sf::RenderTarget& target);

protected:
	EntityList Entities;

	AssetManager Assets;

};