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

void Scene::BeginScene(sf::RenderTarget& target)
{	
	InitializeSceneView(target);

	//testing
	Assets = AssetManager();
	Assets.Load(Textures::Grass, "faea");
	Assets.Load(Textures::Wall, "Assets/ajax.png");

	std::string fuck = ADKEditorMetaRegistry::Identifiers[1];
	Entity* E = ADKEditorMetaRegistry::CreateNewEntity(fuck);

	E->SetSpriteTexture(Assets.Get(Textures::Wall));
	E->SetDepth(2);
	Entities.Add(E);

	//auto NewItem = new ExampleEntity();//(30, 30);
	//NewItem->SetSpriteTexture(Assets.Get(Textures::Wall));
	//Entities.Add(NewItem);
	std::cout << typeid(E).name() << std::endl;

	auto NewItem2 = new Entity(10, 10);
	NewItem2->SetSpriteTexture(Assets.Get(Textures::Grass));
	NewItem2->SetDepth(2);
	Entities.Add(NewItem2);
	std::cout << typeid(NewItem2).name() << std::endl;
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