#pragma once

#include <SFML/Graphics.hpp>

#include "EntityList.h"

#define LOG(string) std::cout << string << std::endl

// Struct for Scene View properties
struct FViewConfig
{
	// Todo: Defaults for scene view
	float CenterX = 640.f;
	float CenterY = 360.f;
	float SizeX = 1280.f;
	float SizeY = 720.f;
	float Rotation = 0.f;
	float Zoom = 1.f;
};

class Scene
{
public:
	Scene();

	// Called on scene creation. Must override.
	virtual void BeginScene(sf::RenderWindow& window) = 0;

	virtual void EndScene(sf::RenderWindow& window);

	virtual void ProcessEvents(sf::Event& event);

	// Called before update.
	virtual void PreUpdate(float deltaTime);

	// Update the entities in this scene. Must override.
	virtual void Update(float deltaTime) = 0;

	// Called after update.
	virtual void PostUpdate(float deltaTime);

	// Called before rendering. (probably used to set view and stuff)
	virtual void PreRender(sf::RenderWindow& window);

	// Draw the entities in this scene. Must override.
	virtual void Render(sf::RenderWindow& window) = 0;

	// Called after rendering.
	virtual void PostRender(sf::RenderWindow& window);

protected:
	// Initialize SceneView based on ViewConfig. Set RenderTarget's view to SceneView
	virtual void InitializeSceneView(sf::RenderWindow& window);

public:
	// The entities contained in this scene. You don't have to use this.
	EntityList Entities;

protected:
	// View configurations.
	FViewConfig ViewConfig;
	// View to use in scene. Used to set the window's view.
	sf::View SceneView;

};