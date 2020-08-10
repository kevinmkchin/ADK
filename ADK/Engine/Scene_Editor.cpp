#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

#include "MoreColors.h"
#include "Scene_Editor.h"
#include "AssetManager.h"
#include "../ADKEditorMetaRegistry.h"

FEditorConfig::FEditorConfig()
	: GridSizeX(32)
	, GridSizeY(32)
	, GridColor(sf::Color::White)
	, bShowGrid(true)
	, bSnapToGrid(false)

	, SelectionColor(sf::Color::Green)
{
}

Scene_Editor::Scene_Editor()
	: DefaultEditorConfig(FEditorConfig())
	, ActiveEditorConfig(DefaultEditorConfig)
	, renderWindowPtr(nullptr)
	, EntitySelectedForCreation(nullptr)
	, EntitySelectedForProperties(nullptr)
	, bgRect(sf::FloatRect(0.03f * 1600, 0.04f * 900, 0.78f * 1600, 0.738f * 900))
	, bEntityDrag(false)
	, bMouseDrag(false)
	, lastMousePos(sf::Vector2f())
	, currTool(TOOL_PLACE)
{
}

void Scene_Editor::BeginScene(sf::RenderWindow& window)
{
	LOG("Began Editor Scene")

	// TODO load editor config from ini

	// Initialize ImGui::SFML process
	ImGui::SFML::Init(window);
	renderWindowPtr = &window;

#pragma region ImGuiStyle
	ImGuiStyle * style = &ImGui::GetStyle();
	//style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f;
	//style->FramePadding = ImVec2(5, 5);
	//style->FrameRounding = 4.0f;
	//style->ItemSpacing = ImVec2(12, 8);
	//style->ItemInnerSpacing = ImVec2(8, 6);
	//style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 17.0f;
	style->ScrollbarRounding = 9.0f;
	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4((float)23 / (float)255, (float)29 / (float)255, (float)34 / (float)255, 1.00f);//ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.467f, 0.533f, 0.6f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.467f, 0.6f, 0.6f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.251f, 0.267f, 1.00f);
#pragma endregion

	// Setup view
	SceneView.setViewport(sf::FloatRect(0.03f, 0.04f, 0.78f, 0.738f));
	InitializeSceneView(window);

	// Make entity of every type to display to editor 
	Assets = AssetManager();
	Assets.Load(Textures::Default);
	Assets.Load(Textures::Ajax);
	Assets.Load(Textures::Stone);
	for (auto iter = ADKEditorMetaRegistry::Identifiers.begin(); iter != ADKEditorMetaRegistry::Identifiers.end(); ++iter)
	{
		Entity* created = ADKEditorMetaRegistry::CreateNewEntity(*iter);
		created->EntityId = *iter;
		// At this point, Identifiers[0] represents the id of the entity type of entity at EntityTypes.at(0)
		// For each entity created, set their sprite texture with the specified default texture enum
		created->SetSpriteTexture(Assets.Get(created->GetTextureId()));
		EntityTypes.add(created);
	}
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
	if (bgRect.contains(sf::Vector2f(sf::Mouse::getPosition(*renderWindowPtr))) && event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed)
	{
		if (EntitySelectedForCreation != nullptr)
		{
			Entity* created = ADKEditorMetaRegistry::CreateNewEntity(EntitySelectedForCreation->EntityId);
			created->EntityId = EntitySelectedForCreation->EntityId;
			created->SetSpriteTexture(Assets.Get(created->GetTextureId()));
			// get the current mouse position in the window
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
			// convert it to world coordinates
			sf::Vector2f worldPos = (*renderWindowPtr).mapPixelToCoords(pixelPos);
			// Calculate amount to subtract if snapping to grid
			int sX = 0;
			int sY = 0;
			if (ActiveEditorConfig.bSnapToGrid)
			{
				sX = (int)worldPos.x % ActiveEditorConfig.GridSizeX;
				if (worldPos.x < 0)
				{
					sX = ActiveEditorConfig.GridSizeX + sX;
				}
				std::cout << "sX " << sX << std::endl;
				sY = (int)worldPos.y % ActiveEditorConfig.GridSizeY;
				if (worldPos.y < 0)
				{
					sY = ActiveEditorConfig.GridSizeY + sY;
				}
				std::cout << "sY " << sY << std::endl;
			}
			// Set entity's position
			created->SetPosition((float) ((int)worldPos.x - sX), (float) ((int)worldPos.y - sY));
			Entities.add(created);
			SetEntitySelectedForProperties(created);
			bEntityDrag = true;

			std::cout << "Placed " << created->EntityId << " at x: " << worldPos.x << "  y: " << worldPos.y << std::endl;
		}
	}

	// Entity drag
	if (event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonReleased)
	{
		bEntityDrag = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		bEntityDrag = true;
	}
	else if (sf::Event::KeyReleased && event.key.code == sf::Keyboard::LAlt)
	{
		bEntityDrag = false;
	}

	// Mouse view drag
	if (event.mouseButton.button == sf::Mouse::Middle)
	{
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (bMouseDrag == false)
			{
				// Set last mouse pos to current mouse pos
				sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
				lastMousePos = (*renderWindowPtr).mapPixelToCoords(pixelPos);
			}
			bMouseDrag = true;
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			bMouseDrag = false;
		}
	}

	// Mouse view zoom
	if (bgRect.contains(sf::Vector2f(sf::Mouse::getPosition(*renderWindowPtr))) && bMouseDrag == false && event.type == sf::Event::MouseWheelMoved)
	{
		SceneView.zoom(event.mouseWheel.delta > 0 ? 0.9f : 1.1f);
		renderWindowPtr->setView(SceneView);
	}

	// Scene move
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
	{
		float xM = 0;
		float yM = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			xM -= 10;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			xM += 10;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			yM -= 10;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			yM += 10;
		}
		SceneView.move(sf::Vector2f(xM, yM));
		renderWindowPtr->setView(SceneView);
	}

	// Entity Arrow move
	if (EntitySelectedForProperties != nullptr)
	{
		float xM = 0;
		float yM = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			xM -= 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			xM += 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			yM -= 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			yM += 1;
		}
		sf::Vector2f pos = EntitySelectedForProperties->GetPosition();
		pos.x += xM;
		pos.y += yM;
		EntitySelectedForProperties->SetPosition(pos);
	}
}

void Scene_Editor::PreUpdate(float deltaTime)
{

}

void Scene_Editor::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	// Move entity if dragging it around
	if (bEntityDrag && EntitySelectedForProperties != nullptr)
	{
		sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
		sf::Vector2f worldPos = (*renderWindowPtr).mapPixelToCoords(pixelPos);
		// Calculate amount to subtract if snapping to grid
		int sX = 0;
		int sY = 0;
		if (ActiveEditorConfig.bSnapToGrid)
		{
			sX = (int)worldPos.x % ActiveEditorConfig.GridSizeX;
			if (worldPos.x < 0)
			{
				sX = ActiveEditorConfig.GridSizeX + sX;
			}
			std::cout << "sX " << sX << std::endl;
			sY = (int)worldPos.y % ActiveEditorConfig.GridSizeY;
			if (worldPos.y < 0)
			{
				sY = ActiveEditorConfig.GridSizeY + sY;
			}
			std::cout << "sY " << sY << std::endl;
		}
		// Set entity's position
		EntitySelectedForProperties->SetPosition((float)((int)worldPos.x - sX), (float)((int)worldPos.y - sY));
	}

	// Move view if dragging view
	if (bMouseDrag)
	{		
		sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
		sf::Vector2f worldPos = (*renderWindowPtr).mapPixelToCoords(pixelPos);
		sf::Vector2f delta = worldPos - lastMousePos;
		SceneView.move(-delta);
		renderWindowPtr->setView(SceneView);
		lastMousePos = worldPos;
	}

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

}

void Scene_Editor::Render(sf::RenderWindow& window)
{
	Scene::Render(window);

	// Render Grid
	if (ActiveEditorConfig.bShowGrid)
	{
		sf::Vector2i topLeftCornerInPixels(48, 36);
		sf::Vector2f topLeftWorld = (*renderWindowPtr).mapPixelToCoords(topLeftCornerInPixels);

		sf::Vector2i botRightCornerInPixels(1297, 700);
		sf::Vector2f botRightWorld = (*renderWindowPtr).mapPixelToCoords(botRightCornerInPixels);

		int gridRangeX = 6400 / ActiveEditorConfig.GridSizeX; 
		int gridRangeY = 6400 / ActiveEditorConfig.GridSizeY; 

		for (int x = -gridRangeX; x < gridRangeX; ++x)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f((float) (x * ActiveEditorConfig.GridSizeX), topLeftWorld.y);
			line[0].color = ActiveEditorConfig.GridColor;
			line[1].position = sf::Vector2f((float) (x * ActiveEditorConfig.GridSizeX), botRightWorld.y);
			line[1].color = ActiveEditorConfig.GridColor;
			window.draw(line, 2, sf::Lines);
		}

		for (int y = -gridRangeY; y < gridRangeY; ++y)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f(topLeftWorld.x, (float) (y * ActiveEditorConfig.GridSizeY));
			line[0].color = ActiveEditorConfig.GridColor;
			line[1].position = sf::Vector2f(botRightWorld.x, (float) (y * ActiveEditorConfig.GridSizeY));
			line[1].color = ActiveEditorConfig.GridColor;
			window.draw(line, 2, sf::Lines);
		}
	}

	// Show Selection
	if (EntitySelectedForProperties != nullptr)
	{
		float x = EntitySelectedForProperties->GetSprite().getPosition().x;
		float y = EntitySelectedForProperties->GetSprite().getPosition().y;
		float width = (float) EntitySelectedForProperties->GetSprite().getTextureRect().width;
		float height = (float) EntitySelectedForProperties->GetSprite().getTextureRect().height;

		sf::Vertex box[8];
		box[0].position = sf::Vector2f(x, y);
		box[1].position = sf::Vector2f(x + width, y);
		box[2].position = sf::Vector2f(x + width, y + height);
		box[3].position = sf::Vector2f(x, y + height);
		box[4].position = sf::Vector2f(x, y);
		box[5].position = sf::Vector2f(x, y + height);
		box[6].position = sf::Vector2f(x + width, y + height);
		box[7].position = sf::Vector2f(x + width, y);
		for (int i = 0; i < 8; ++i)
		{
			box[i].color = ActiveEditorConfig.SelectionColor;
		}
		window.draw(box, 8, sf::Lines);

		sf::RectangleShape selection;
		sf::Color selectionColor = ActiveEditorConfig.SelectionColor;
		selectionColor.a = 45;
		selection.setFillColor(selectionColor);
		selection.setPosition(x, y);
		selection.setSize(sf::Vector2f(width, height));
		window.draw(selection);
	}
}

void Scene_Editor::PostRender(sf::RenderWindow& window)
{
	// Render ImGui::SFML. All creation of ImGui widgets must happen between ImGui::SFML::Update and ImGui::SFML::Render.
	ImGui::SFML::Render(window);
}

void Scene_Editor::DrawEditorUI()
{
	DrawMenuAndOptionsBarUI();
	DrawEntityPropertyUI();
	DrawEntityTypeUI();
	DrawToolsMenuUI();
}

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

}

void Scene_Editor::DrawEntityPropertyUI()
{
	ImGui::Begin("Entities and Properties", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f(1297.f, 36.f));
	ImGui::SetWindowSize(sf::Vector2f(303.f, 864.f));

	ImGui::BeginTabBar("");
	if (ImGui::BeginTabItem("Properties"))
	{
		// Display properties for currently selected entity

		if (EntitySelectedForProperties != nullptr)
		{
			int x = (int) EntitySelectedForProperties->GetPosition().x;
			int y = (int) EntitySelectedForProperties->GetPosition().y;
			ImGui::InputInt("X Position", &x);
			ImGui::InputInt("Y Position", &y);
			EntitySelectedForProperties->SetPosition((float) x, (float) y);

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
	if (ImGui::BeginTabItem("Entities in Level"))
	{
		// Two columns
		ImGui::Columns(2);

		// Display all entities currently in level
		int selected = -1;
		std::vector<std::string> bruh;
		for (int i = 0; i < Entities.size(); ++i)
		{
			std::string name = std::to_string(i).append(" ");
			name = Entities.at(i)->EntityId.empty() ? name.append(typeid(Entities.at(i)).name()) : name.append(Entities.at(i)->EntityId);
			bruh.push_back(name);
			if (Entities.at(i) == EntitySelectedForProperties)
			{
				selected = i;
			}
		}
		if (ImGui::ListBox("", &selected, bruh))
		{
			SetEntitySelectedForProperties(Entities.at(selected));
		}
		ImGui::NextColumn();
		
		ImGui::Text("Show Tags");
		bool ex = true;
		ImGui::Checkbox("Collider", &ex);
		bool ex2 = true;
		ImGui::Checkbox("Enemies", &ex2);
		bool ex3 = true;
		ImGui::Checkbox("Walls", &ex3);
		bool ex4 = true;
		ImGui::Checkbox("Background", &ex4);
		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::Text("Only show entities");
		ImGui::Text("with these tags in");
		ImGui::Text("the level viewer?");
		bool bShowOnlyTags;
		ImGui::Checkbox("", &bShowOnlyTags);
		ImGui::NextColumn();

		std::string num = std::to_string(Entities.size()).append(" entities in level.");
		const char * numInCharPtr = num.c_str();
		ImGui::Text(numInCharPtr);

		// Delete Button
		if (ImGui::Button("Delete Selected"))
		{
			if (EntitySelectedForProperties != nullptr)
			{
				Entities.removeAndDestroy(EntitySelectedForProperties);
				SetEntitySelectedForProperties(nullptr);
			}
		}

		// todo Entities in the level count

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

	ImGui::PushItemWidth(90.f);
	ImGui::InputInt("Grid Size X", &ActiveEditorConfig.GridSizeX);
	ImGui::SameLine();
	ImGui::InputInt("Grid Size Y", &ActiveEditorConfig.GridSizeY);
	ImGui::SameLine();
	ImGui::Checkbox("Show Grid", &ActiveEditorConfig.bShowGrid);
	ImGui::SameLine();
	ImGui::Checkbox("Snap to Grid", &ActiveEditorConfig.bSnapToGrid);
	ImGui::SameLine();

	ImGui::PushItemWidth(130.f);
	ImColor gridCol = MoreColors::SFColorToImColor(ActiveEditorConfig.GridColor);
	if (ImGui::ColorEdit3("Grid Color", (float*)&gridCol, ImGuiColorEditFlags_NoInputs))
	{
		EntitySelectedForCreation = nullptr;
	}
	ActiveEditorConfig.GridColor = MoreColors::ImColorToSFColor(gridCol);
	ImGui::SameLine();

	ImColor selCol = MoreColors::SFColorToImColor(ActiveEditorConfig.SelectionColor);
	if (ImGui::ColorEdit3("Select Color", (float*)&selCol, ImGuiColorEditFlags_NoInputs))
	{
		EntitySelectedForCreation = nullptr;
	}
	ActiveEditorConfig.SelectionColor = MoreColors::ImColorToSFColor(selCol);
	ImGui::SameLine();

	// RESET VIEW button


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

	// Pick entitytype from level

	ImGui::Button("Place Single", sf::Vector2f(32.f, 32.f));
	ImGui::Button("Paint ", sf::Vector2f(32.f, 32.f));

	ImGui::End();
}

void Scene_Editor::SetEntitySelectedForProperties(Entity* newSelection)
{
	// Clear the modified sprite color of the last entity
	if (EntitySelectedForProperties != nullptr)
	{
		EntitySelectedForProperties->GetSprite().setColor(sf::Color::White);
	}
	// Set new entity
	EntitySelectedForProperties = newSelection;
	// Modify sprite color of new entity
	if (EntitySelectedForProperties != nullptr)
	{
		EntitySelectedForProperties->GetSprite().setColor(sf::Color::White); // set to whatever modified color u want
	}
}