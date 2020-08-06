#include <iostream>
#include "Scene.h"

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

void Scene::BeginScene(sf::RenderTarget& target)
{	
	InitializeSceneView(target);

	//testing
	Assets = AssetManager();
	Assets.Load(Textures::Grass, "faea");
	Assets.Load(Textures::Wall, "Assets/ajax.png");

	Entity* NewItem = new Entity(30, 30);
	NewItem->SetSpriteTexture(Assets.Get(Textures::Wall));
	NewItem->SetDepth(2);
	Entities.Add(NewItem);

	NewItem = new Entity(10, 10);
	NewItem->SetSpriteTexture(Assets.Get(Textures::Grass));
	NewItem->SetDepth(2);
	Entities.Add(NewItem);
	//
}

void Scene::EndScene(sf::RenderTarget& target)
{
	// Reset to default view
	target.setView(target.getDefaultView());
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

void Scene::PreRender(sf::RenderTarget& target)
{

}

void Scene::Render(sf::RenderTarget& target)
{
	Entities.Render(target);
}

void Scene::PostRender(sf::RenderTarget& target)
{

}

void Scene::InitializeSceneView(sf::RenderTarget& target)
{
	// Initialize SceneView
	SceneView.setCenter(sf::Vector2f(ViewConfig.CenterX, ViewConfig.CenterY));
	SceneView.setSize(sf::Vector2f(ViewConfig.SizeX, ViewConfig.SizeY));
	SceneView.setRotation(ViewConfig.Rotation);
	SceneView.zoom(ViewConfig.Zoom);
	// Set view
	target.setView(SceneView);
}