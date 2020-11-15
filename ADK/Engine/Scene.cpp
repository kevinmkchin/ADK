#include "Scene.h"
#include "../ADKEditorMetaRegistry.h"

sf::View nullview;

Scene::Scene()
	: entities(EntityList())
{
}

void Scene::begin_scene(sf::RenderWindow& window)
{	
	// Initialize scene view based on view config
	initialize_scene_view(window);
	// Set key presses to poll repeatedly
	window.setKeyRepeatEnabled(true);
}

void Scene::end_scene(sf::RenderWindow& window)
{
	// Reset to default view
	window.setView(window.getDefaultView());
}

void Scene::process_events(sf::Event& event)
{

}

void Scene::update_pre(float deltaTime)
{

}

void Scene::update(float deltaTime)
{

}

void Scene::update_post(float deltaTime)
{

}

void Scene::render_pre(sf::RenderWindow& window)
{

}

void Scene::render(sf::RenderWindow& window)
{

}

void Scene::render_post(sf::RenderWindow& window)
{

}

void Scene::initialize_scene_view(sf::RenderWindow& window)
{
	sf::View view_to_set = window.getView();
	// Initialize SceneView
	view_to_set.setCenter(sf::Vector2f(view_config.center_x, view_config.center_y));
	view_to_set.setSize(sf::Vector2f(view_config.size_x, view_config.size_y));
	view_to_set.setRotation(view_config.rotation);
	view_to_set.zoom(view_config.zoom);
	// Set view
	window.setView(view_to_set);
}