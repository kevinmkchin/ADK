#include <iostream>
#include "Scene.h"

Scene::Scene()
	: Entities(EntityList())
{
	//testing
	Assets = AssetManager();
	Assets.Load(Textures::Grass, "faea");
	Entity* NewItem = new Entity(10,10);
	NewItem->SetSpriteTexture(Assets.Get(Textures::Grass));
	Entities.Add(NewItem);

}

void Scene::PreUpdate(float deltaTime)
{
	std::cout << "Scene Pre Update" << std::endl;
}

void Scene::Update(float deltaTime)
{
	std::cout << "Scene Update" << std::endl;
	Entities.Update(deltaTime);
}

void Scene::PostUpdate(float deltaTime)
{
	std::cout << "Scene Post Update" << std::endl;
}

void Scene::PreRender(sf::RenderTarget& target)
{
	std::cout << "Scene Pre Render" << std::endl;
}

void Scene::Render(sf::RenderTarget& target)
{
	std::cout << "Scene Render" << std::endl;
	Entities.Render(target);

	sf::Sprite fuck;
	fuck.setTexture(Assets.Get(Textures::Grass));
	fuck.setPosition(100, 100);
	target.draw(fuck);
}

void Scene::PostRender(sf::RenderTarget& target)
{
	std::cout << "Scene Post Render" << std::endl;
}