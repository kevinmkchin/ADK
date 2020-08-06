#pragma once

#include <SFML/Graphics.hpp>
#include "EntityList.h"
#include "AssetManager.h"

// Struct for Scene View properties
struct FViewConfig
{
	float CenterX;
	float CenterY;
	float SizeX;
	float SizeY;
	float Rotation;
	float Zoom;

	// Defaults
	FViewConfig();
};

class Scene
{
public:
	Scene();

	virtual void BeginScene(sf::RenderTarget& target);

	virtual void EndScene(sf::RenderTarget& target);

	virtual void PreUpdate(float deltaTime);

	virtual void Update(float deltaTime);

	virtual void PostUpdate(float deltaTime);

	// Called before render (probably used to set view and stuff)
	virtual void PreRender(sf::RenderTarget& target);

	// Draw the entities in this scene
	virtual void Render(sf::RenderTarget& target);

	// Stuff to do after rendering
	virtual void PostRender(sf::RenderTarget& target);

protected:
	// Initialize SceneView based on ViewConfig. Set RenderTarget's view to SceneView
	virtual void InitializeSceneView(sf::RenderTarget& target);

protected:
	EntityList Entities;

	AssetManager Assets;

	// View
	FViewConfig ViewConfig;
	sf::View SceneView;

};