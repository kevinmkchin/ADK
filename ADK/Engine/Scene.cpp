#include <iostream>
#include "Scene.h"
#include "../ExampleEntity.h"
#include "../ADKEditorMetaRegistry.h"

FViewConfig::FViewConfig()
	: CenterX(640.f)
	, CenterY(360.f)
	, SizeX(1280.f)
	, SizeY(720.f)
	, Rotation(0.f)
	, Zoom(1.f)
{
}

Scene::Scene()
	: Entities(EntityList())
{

}

void Scene::BeginScene(sf::RenderWindow& window)
{	
	InitializeSceneView(window);

}

void Scene::EndScene(sf::RenderWindow& window)
{
	// Reset to default view
	window.setView(window.getDefaultView());
}

void Scene::ProcessEvents(sf::Event& event)
{

}

void Scene::PreUpdate(float deltaTime)
{

}

void Scene::Update(float deltaTime)
{

	Entities.Update(deltaTime);
}

void Scene::PostUpdate(float deltaTime)
{

}

void Scene::PreRender(sf::RenderWindow& window)
{

}

void Scene::Render(sf::RenderWindow& window)
{
	Entities.Render(window);
}

void Scene::PostRender(sf::RenderWindow& window)
{

}

void Scene::InitializeSceneView(sf::RenderWindow& window)
{
	// Initialize SceneView
	SceneView.setCenter(sf::Vector2f(ViewConfig.CenterX, ViewConfig.CenterY));
	SceneView.setSize(sf::Vector2f(ViewConfig.SizeX, ViewConfig.SizeY));
	SceneView.setRotation(ViewConfig.Rotation);
	SceneView.zoom(ViewConfig.Zoom);
	// Set view
	window.setView(SceneView);
}