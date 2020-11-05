#pragma once

#include <SFML/Graphics.hpp>

#include "EntityList.h"

#define LOG(string) std::cout << string << std::endl

// Struct for Scene View properties
struct FViewConfig
{
	// Todo: Defaults for scene view
	float center_x = 640.f;
	float center_y = 360.f;
	float size_x = 1280.f;
	float size_y = 720.f;
	float rotation = 0.f;
	float zoom = 1.f;
};

class Scene
{
public:
	Scene();

	// Called on scene creation. Must override.
	virtual void begin_scene(sf::RenderWindow& window) = 0;

	virtual void end_scene(sf::RenderWindow& window);

	virtual void process_events(sf::Event& event);

	// Called before update.
	virtual void update_pre(float deltaTime);

	// Update the entities in this scene. Must override.
	virtual void update(float deltaTime) = 0;

	// Called after update.
	virtual void update_post(float deltaTime);

	// Called before rendering. (probably used to set view and stuff)
	virtual void render_pre(sf::RenderWindow& window);

	// Draw the entities in this scene. Must override.
	virtual void render(sf::RenderWindow& window) = 0;

	// Called after rendering.
	virtual void render_post(sf::RenderWindow& window);

protected:
	// Initialize SceneView based on ViewConfig. Set RenderTarget's view to SceneView
	virtual void initialize_scene_view(sf::RenderWindow& window);

public:
	// The entities contained in this scene. You don't have to use this.
	EntityList entities;

protected:
	// View configurations.
	FViewConfig view_config;
	// View to use in scene. Used to set the window's view.
	sf::View scene_view;

};