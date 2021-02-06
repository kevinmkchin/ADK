#pragma once

#include <SFML/Graphics.hpp>

#include "EntityList.h"

#define LOG(string) std::cout << string << std::endl

class Engine;

// Struct for Scene View properties
struct FViewConfig
{
	float position_x = 640.f; // sfml view center x
	float position_y = 360.f; // sfml view center y
	float size_x = 1280.f;    // sfml view size x
	float size_y = 720.f;     // sfml view size y
	float rotation = 0.f;     // sfml view rotation
	float zoom = 1.f;         // sfml view zoom
};

class Scene
{
public:
	Scene();

	// Called on scene creation. Must override.
	virtual void begin_scene(sf::RenderWindow& window) = 0;

	// Clean up scene and entities. Must override.
	virtual void end_scene(sf::RenderWindow& window) = 0;

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

	void set_engine_instance(Engine* in_engine);

	// Call to pause or unpause update. pause_in_seconds is how long to pause update for before resuming.
	void pause_update(bool b_pause, float in_pause_in_seconds = -1.f);
	bool is_update_paused() const;
	virtual void on_unpause();

	// Scene specific ImGui for control panel
	virtual void show_scene_debugui();

protected:
	// Initialize SceneView based on ViewConfig. Set RenderTarget's view to SceneView
	virtual void initialize_scene_view(sf::RenderWindow& window);

public:
	/*	The entities contained in this scene. You don't have to use this.
		If you want entities to persist between ADKSaveLoad::load_to_scene calls, then probably don't put them in here. */
	EntityList level_entities;

protected:
	// View configurations.
	FViewConfig view_config;

	// Reference to engine instance
	Engine* engine_ptr;

	// Reference to render window
	sf::RenderWindow* render_window_ptr;

};