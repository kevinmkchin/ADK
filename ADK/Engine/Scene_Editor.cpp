#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

#include "MoreColors.h"
#include "Scene_Editor.h"
#include "AssetManager.h"
#include "../ADKEditorMetaRegistry.h"


void Scene_Editor::BeginScene(sf::RenderWindow& window)
{
	LOG("Began Editor Scene")

	// Initialize ImGui::SFML process
	ImGui::SFML::Init(window);
	renderWindowPtr = &window;

	// Setup view
	SceneView.setViewport(sf::FloatRect(0.03f, 0.04f, 0.78f, 0.738f));
	InitializeSceneView(window);

	background = sf::RectangleShape(sf::Vector2f(1249.f, 664.f));
	background.setFillColor(MC_CHARCOAL);
	background.setPosition(0.f, 0.f);

	// Make entity of every type to display to editor 
	Assets = AssetManager();
	Assets.Load(Textures::Default);
	Assets.Load(Textures::Ajax);
	Assets.Load(Textures::Stone);
	for (auto iter = ADKEditorMetaRegistry::Identifiers.begin(); iter != ADKEditorMetaRegistry::Identifiers.end(); ++iter)
	{
		Entity* created = ADKEditorMetaRegistry::CreateNewEntity(*iter);
		// At this point, Identifiers[0] represents the id of the entity type of entity at EntityTypes.at(0)
		// For each entity created, set their sprite texture with the specified default texture enum
		created->SetSpriteTexture(Assets.Get(created->GetTextureId()));
		EntityTypes.add(created);
	}


	// Testing
	std::string ye = ADKEditorMetaRegistry::Identifiers[1];
	Entity* E = ADKEditorMetaRegistry::CreateNewEntity(ye);
	E->SetSpriteTexture(Assets.Get(Textures::Ajax));
	E->SetDepth(1);
	Entities.add(E);
	//auto NewItem = new ExampleEntity();//(30, 30);
	//NewItem->SetSpriteTexture(Assets.Get(Textures::Wall));
	//Entities.Add(NewItem);
	auto NewItem2 = new Entity(10, 10);
	NewItem2->SetSpriteTexture(Assets.Get(Textures::Default));
	NewItem2->SetDepth(2);
	Entities.add(NewItem2);
	// Testing
}

void Scene_Editor::EndScene(sf::RenderWindow& window)
{
	// Shut down ImGui::SFML process
	ImGui::SFML::Shutdown();

	// Deallocate entity memory
	for (int i = EntityTypes.size() - 1; i != -1; --i)
	{
		EntityTypes.removeAndDestroy(EntityTypes.at(i));
	}
}

void Scene_Editor::ProcessEvents(sf::Event& event)
{
	// Process events/inputs for ImGui::SFML
	ImGui::SFML::ProcessEvent(event);


	// Place with mouse
	sf::Rect<float> bgCol(background.getPosition().x, background.getPosition().y, 
		background.getPosition().x + background.getSize().x, background.getPosition().y + background.getSize().y);
	if (bgCol.contains(sf::Vector2f(sf::Mouse::getPosition(*renderWindowPtr))) && event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed)
	{
		std::cout << "place smth" << std::endl;

		if (EntitySelectedForCreation != nullptr)
		{
			int idIndex = EntityTypes.find(EntitySelectedForCreation);
			std::string id = ADKEditorMetaRegistry::Identifiers[idIndex];
			Entity* created = ADKEditorMetaRegistry::CreateNewEntity(id);
			created->SetSpriteTexture(Assets.Get(created->GetTextureId()));
			created->SetPosition(sf::Vector2f(sf::Mouse::getPosition(*renderWindowPtr)));
			Entities.add(created);
			EntitySelectedForProperties = created;
		}
	}
}

void Scene_Editor::PreUpdate(float deltaTime)
{

}

void Scene_Editor::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	// ImGui::SFML Update
	ImGui::SFML::Update(*renderWindowPtr, sf::seconds(deltaTime));
	// Setup ImGui to draw
	DrawEditorUI();
}

void Scene_Editor::PostUpdate(float deltaTime)
{

}

void Scene_Editor::PreRender(sf::RenderWindow& window)
{
	// Draw background
	window.draw(background);
}

void Scene_Editor::Render(sf::RenderWindow& window)
{
	Scene::Render(window);

	// Render ImGui::SFML. All creation of ImGui widgets must happen between ImGui::SFML::Update and ImGui::SFML::Render.
	ImGui::SFML::Render(window);
}

void Scene_Editor::PostRender(sf::RenderWindow& window)
{

}

void Scene_Editor::DrawEditorUI()
{
	DrawMenuAndOptionsBarUI();
	DrawEntityPropertyUI();
	DrawEntityTypeUI();
	DrawToolsMenuUI();
}

void Scene_Editor::DrawEntityPropertyUI()
{
	ImGui::Begin("Entities and Properties", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f(1297.f, 36.f));
	ImGui::SetWindowSize(sf::Vector2f(303.f, 864.f));

	ImGui::BeginTabBar("");
	if (ImGui::BeginTabItem("Entities"))
	{
		// Display all entities currently in level
		for (int i = 0; i < Entities.size(); ++i)
		{
			ImGui::Text(typeid(Entities.at(i)).name());
		}

		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Properties"))
	{
		// Display properties for currently selected entity

		if (EntitySelectedForProperties != nullptr)
		{
			int x = EntitySelectedForProperties->GetPosition().x;
			int y = EntitySelectedForProperties->GetPosition().y;
			ImGui::InputInt("X Position", &x);
			ImGui::InputInt("Y Position", &y);
			EntitySelectedForProperties->SetPosition(x, y);

			float angle = EntitySelectedForProperties->GetRotation();
			ImGui::InputFloat("Rotation", &angle);
			EntitySelectedForProperties->SetRotation(angle);

			ImGui::Text("Scale");

			int depth = EntitySelectedForProperties->GetDepth();
			ImGui::InputInt("Depth", &depth);
			EntitySelectedForProperties->SetDepth(depth);
		}

		ImGui::Separator();

		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();

	ImGui::End();
}

void Scene_Editor::DrawEntityTypeUI()
{
	ImGui::Begin("Entity Types Available", nullptr, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(sf::Vector2f(0.f, 700.f));
	ImGui::SetWindowSize(sf::Vector2f(1297.f, 200.f));
	//std::cout << std::to_string(ImGui::GetWindowPos().x) + " position " + std::to_string(ImGui::GetWindowPos().y) << std::endl;
	//std::cout << std::to_string(ImGui::GetWindowSize().x) + " size " + std::to_string(ImGui::GetWindowSize().y) << std::endl;

	// Display all available entity types
	for (int i = 0; i < EntityTypes.size(); ++i)
	{
		ImGui::BeginGroup();
		{
			sf::Sprite entitySprite = EntityTypes.at(i)->GetSprite();
			const char* EntityId = ADKEditorMetaRegistry::Identifiers[i].c_str();

			if (ImGui::ImageButton(entitySprite, sf::Vector2f(50.f, 50.f)))
			{
				EntitySelectedForCreation = EntityTypes.at(i);
				std::cout << TextureManager::TexturePaths.at(EntitySelectedForCreation->GetTextureId()) << std::endl;
			}
			ImGui::Text(EntityId);

			++i;
			if (i < EntityTypes.size())
			{
				entitySprite = EntityTypes.at(i)->GetSprite();
				EntityId = ADKEditorMetaRegistry::Identifiers[i].c_str();
				if (ImGui::ImageButton(entitySprite, sf::Vector2f(50.f, 50.f)))
				{
					EntitySelectedForCreation = EntityTypes.at(i);
					std::cout << TextureManager::TexturePaths.at(EntitySelectedForCreation->GetTextureId()) << std::endl;
				}
				ImGui::Text(EntityId);
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
	}

	ImGui::End();
}

void Scene_Editor::DrawMenuAndOptionsBarUI()
{
	ImGui::Begin("Menu and  Editor Options", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f(0.f, 0.f));
	ImGui::SetWindowSize(sf::Vector2f(1600.f, 36.f));

	ImGui::Button("Save");
	ImGui::SameLine();

	ImGui::Button("Load");
	ImGui::SameLine(400.f);

	ImGui::Button("Toggle Grid");
	ImGui::SameLine();

	//ImGui::PushItemWidth(50.f);
	//char GridSizeX[5] = ""; //problematic
	//ImGui::InputText("32", GridSizeX, 5);
	//ImGui::SameLine();

	//char GridSizeY[5] = ""; //problematic
	//ImGui::InputText("32", GridSizeY, 5);
	//ImGui::SameLine();

	//bool bSnapToGrid = false; //problematic
	//ImGui::Checkbox("Snap to Grid", &bSnapToGrid);


	ImGui::End();
}

void Scene_Editor::DrawToolsMenuUI()
{
	ImGui::Begin("Tools Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f(0.f, 36.f));
	ImGui::SetWindowSize(sf::Vector2f(48.f, 664.f));

	// Selection tool

	// Click tool

	// Brush tool

	ImGui::Button("Place Single", sf::Vector2f(32.f, 32.f));
	ImGui::Button("Paint ", sf::Vector2f(32.f, 32.f));

	ImGui::End();
}