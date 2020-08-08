#include <iostream>

#include "Scene_Editor.h"
#include "../ADKEditorMetaRegistry.h"


void Scene_Editor::BeginScene(sf::RenderTarget& target)
{
	LOG("Began Editor Scene")
	InitializeSceneView(target);

	// Make entity of every type to display to editor 
	for (auto iter = ADKEditorMetaRegistry::Identifiers.begin(); iter != ADKEditorMetaRegistry::Identifiers.end(); ++iter)
	{
		EntityTypes.push_back(ADKEditorMetaRegistry::CreateNewEntity(*iter));
		// At this point, Identifiers[0] represents the id of the entity type of entity at EntityTypes[0]
	}
	
	std::cout << "Num entity types: " + EntityTypes.size() << std::endl;
}

void Scene_Editor::PreUpdate(float deltaTime)
{
	std::cout << "Sub PreUpdate" << std::endl;
}

void Scene_Editor::Update(float deltaTime)
{
	Scene::Update(deltaTime);
	std::cout << "Sub Update " << std::endl;
}

void Scene_Editor::PostUpdate(float deltaTime)
{
	std::cout << "Sub PostUpdate" << std::endl;
}

void Scene_Editor::PreRender(sf::RenderTarget& target)
{
	std::cout << "Sub PreRender" << std::endl;
}

void Scene_Editor::Render(sf::RenderTarget& target)
{
	Scene::Render(target);
	std::cout << "Sub Render" << std::endl;
}

void Scene_Editor::PostRender(sf::RenderTarget& target)
{
	std::cout << "Sub PostRender" << std::endl;
}