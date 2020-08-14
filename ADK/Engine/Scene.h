#pragma once

#include <SFML/Graphics.hpp>

#include "EntityList.h"

#define LOG(string) std::cout << string << std::endl

// Struct for Scene View properties
struct FViewConfig
{
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

	virtual void BeginScene(sf::RenderWindow& window);

	virtual void EndScene(sf::RenderWindow& window);

	virtual void ProcessEvents(sf::Event& event);

	virtual void PreUpdate(float deltaTime);

	virtual void Update(float deltaTime);

	virtual void PostUpdate(float deltaTime);

	// Called before render (probably used to set view and stuff)
	virtual void PreRender(sf::RenderWindow& window);

	// Draw the entities in this scene
	virtual void Render(sf::RenderWindow& window);

	// Stuff to do after rendering
	virtual void PostRender(sf::RenderWindow& window);

protected:
	// Initialize SceneView based on ViewConfig. Set RenderTarget's view to SceneView
	virtual void InitializeSceneView(sf::RenderWindow& window);

protected:
	EntityList Entities;

	// View
	FViewConfig ViewConfig;
	sf::View SceneView;

};