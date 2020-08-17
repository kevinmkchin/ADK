#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

#include "MoreColors.h"
#include "Scene_Editor.h"
#include "ADKSaveLoad.h"
#include "ADKAssets.h"
#include "../ADKEditorMetaRegistry.h"

Scene_Editor::Scene_Editor()
	: DefaultEditorConfig(FEditorConfig())
	, ActiveEditorConfig(DefaultEditorConfig)
	, renderWindowPtr(nullptr)
	, EntitySelectedForCreation(nullptr)
	, EntitySelectedForProperties(nullptr)
	, bgRect(sf::FloatRect(0.03f * 1600, 0.04f * 900, 0.78f * 1600, 0.738f * 900))
	, bBrushEnabled(false)
	, bEntityDrag(false)
	, bMouseDrag(false)
	, zoomFactor(1.f)
	, bTextureShow(false)
	, lastMousePos(sf::Vector2f())
	, currTool(TOOL_SELECTION)
	, defaultCopyPasteTimer(0.5f)
	, copyPasteTimer(defaultCopyPasteTimer)
	, bShowConfig(false)
{
}

void Scene_Editor::BeginScene(sf::RenderWindow& window)
{
	LOG("Began Editor Scene");

	// TODO load editor config from ini

	// Setup big grid
	DefaultEditorConfig.BigGridX = (int) ViewConfig.SizeX;
	DefaultEditorConfig.BigGridY = (int) ViewConfig.SizeY;
	// Setup window values
	UpdateEditorConfigWithWindow(window);

	// Load button textures
	selectButton.loadFromFile("Assets/adk/button_selection.png");
	placeButton.loadFromFile("Assets/adk/button_place.png");
	brushButton.loadFromFile("Assets/adk/button_brush.png");
	pickerButton.loadFromFile("Assets/adk/button_picker.png");

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
	InitializeSceneView(window);

	// Make entity of every type to display to editor
	for (auto iter = ADKEditorMetaRegistry::Identifiers.begin(); iter != ADKEditorMetaRegistry::Identifiers.end(); ++iter)
	{
		Entity* created = ADKEditorMetaRegistry::CreateNewEntity(*iter);
		created->LoadDefaultTexture();
		created->EntityId = *iter;
		EntityTypes.add(created);
		// At this point, Identifiers[0] represents the id of the entity type of entity at EntityTypes.at(0)
	}

	textureDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	textureDialog.SetTitle("Choose a texture to use");
	textureDialog.SetTypeFilters({ ".png", ".jpg" });

	//window.setKeyRepeatEnabled(false);
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

	// Resized. Update Editor Config window values.
	if (event.type == sf::Event::Resized)
	{
		UpdateEditorConfigWithWindow(*renderWindowPtr);
	}

	// Shortcuts
	if (sf::Event::KeyPressed)
	{
		if (bTypingLevelID == false)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				currTool = TOOL_SELECTION;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				currTool = TOOL_PLACE;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				currTool = TOOL_BRUSH;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				currTool = TOOL_PICKER;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				ActiveEditorConfig.bShowGrid = !ActiveEditorConfig.bShowGrid;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
			{
				ActiveEditorConfig.bSnapToGrid = !ActiveEditorConfig.bSnapToGrid;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
		{
			if (EntitySelectedForProperties != nullptr)
			{
				Entities.removeAndDestroy(EntitySelectedForProperties);
				SetEntitySelectedForProperties(nullptr);
			}
		}
	}

	// Editor features that should only happen when sprite sheet viewer is closed.
	if (bTextureShow == false && bShowConfig == false && textureDialog.IsOpened() == false)
	{
		// Select entity
		if (currTool == TOOL_SELECTION && event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed)
		{
			sf::IntRect viewWindow(ActiveEditorConfig.TopLeftPixel.x, ActiveEditorConfig.TopLeftPixel.y,
				ActiveEditorConfig.BotRightPixels.x - ActiveEditorConfig.TopLeftPixel.x, ActiveEditorConfig.BotRightPixels.y - ActiveEditorConfig.TopLeftPixel.y);
			for (int i = Entities.size() - 1; i > -1; --i)
			{
				Entity* at = Entities.at(i);
				sf::IntRect spr = at->GetSprite().getTextureRect();
				sf::FloatRect mouseCol(at->GetPosition().x, at->GetPosition().y, (float)spr.width * at->GetScale(), (float)spr.height * at->GetScale());

				sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
				sf::Vector2f worldPos = (*renderWindowPtr).mapPixelToCoords(pixelPos);
				if (mouseCol.contains(worldPos) 
					&& at->GetDepth() >= ActiveEditorConfig.depthFilterLowerBound 
					&& at->GetDepth() <= ActiveEditorConfig.depthFilterUpperBound 
					&& viewWindow.contains(pixelPos))
				{
					SetEntitySelectedForProperties(at);
					if (EntitySelectedForProperties == at) // This entity is already clicked on
					{
						bEntityDrag = true;
						sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
						sf::Vector2f worldPos = (*renderWindowPtr).mapPixelToCoords(pixelPos);
						sf::Vector2f entPos = EntitySelectedForProperties->GetPosition();
						entityDragOffset = worldPos - entPos;
						return;
						//continue; // see if theres anything under it that we are trying to select
					}
					return; // don't do the rest of the inputs
				}
			}
		}

		// Place entity
		if (bgRect.contains(sf::Vector2f(sf::Mouse::getPosition(*renderWindowPtr))) && event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed)
		{
			if (EntitySelectedForCreation != nullptr)
			{
				// Single place entity
				if (currTool == TOOL_PLACE)
				{
					BrushPlaceHelper();

					bEntityDrag = true;
				}

				// Brush place entity
				if (currTool == TOOL_BRUSH)
				{
					bBrushEnabled = true;
				}
			}
		}

		// Mouse view zoom
		if (bgRect.contains(sf::Vector2f(sf::Mouse::getPosition(*renderWindowPtr))) && bMouseDrag == false && event.type == sf::Event::MouseWheelMoved)
		{
			zoomFactor *= event.mouseWheel.delta > 0 ? 0.9f : 1.1f;
			SceneView.zoom(event.mouseWheel.delta > 0 ? 0.9f : 1.1f);
			renderWindowPtr->setView(SceneView);
		}
	
		// Entity drag with Left Alt
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			bEntityDrag = true;
		}
		else if (sf::Event::KeyReleased && event.key.code == sf::Keyboard::LAlt)
		{
			bEntityDrag = false;
		}

		// Copy
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			if (EntitySelectedForProperties != nullptr)
			{
				Entity::Copy(copiedEntity, *EntitySelectedForProperties);
			}
		}

		// Paste
		if (copyPasteTimer < 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		{
			// Get the current mouse position in the window
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
			// Convert it to world coordinates
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
				sY = (int)worldPos.y % ActiveEditorConfig.GridSizeY;
				if (worldPos.y < 0)
				{
					sY = ActiveEditorConfig.GridSizeY + sY;
				}
			}

			// Get would-be position of a new entity
			int posX = ((int)worldPos.x) - sX;
			int posY = ((int)worldPos.y) - sY;

			// Create a new entity
			Entity* created = ADKEditorMetaRegistry::CreateNewEntity(copiedEntity.EntityId);
			Entity::Copy(*created, copiedEntity);
			created->SetPosition((float)posX, (float)posY);
			// Add the entity to this scene/level editor's entity list
			Entities.add(created);
			// Set this entity to be selected
			SetEntitySelectedForProperties(created);

			copyPasteTimer = defaultCopyPasteTimer;
		}
	}

	// Left mouse released (Stop entity drag & disable brush placement & empty brush visited positions
	if (event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonReleased)
	{
		bEntityDrag = false;
		entityDragOffset = sf::Vector2f(0.f,0.f);
		// Empty the brush tools visited positions cache
		bBrushEnabled = false;
		if (BrushVisitedPositions.empty() == false)
		{
			BrushVisitedPositions.clear();
		}
	}

	// Mouse view drag
	if (event.mouseButton.button == sf::Mouse::Middle)
	{
		if (event.type == sf::Event::MouseButtonPressed)
		{

			// Set last mouse pos to current mouse pos
			lastMousePos = sf::Mouse::getPosition(*renderWindowPtr);
			bMouseDrag = true;
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			bMouseDrag = false;
		}
	}

	// Scene move with Arrows
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

	// Entity move with Arrows
	if (EntitySelectedForProperties != nullptr && bTypingLevelID == false)
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

	// Call brush place
	if (currTool == TOOL_BRUSH && bBrushEnabled)
	{
		BrushPlaceHelper();
	}
	else
	{
		bBrushEnabled = false;
	}

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
			int gridSubX = (int) entityDragOffset.x / ActiveEditorConfig.GridSizeX;
			int gridSubY = (int) entityDragOffset.y / ActiveEditorConfig.GridSizeY;
			worldPos.x -= gridSubX * ActiveEditorConfig.GridSizeX;
			worldPos.y -= gridSubY * ActiveEditorConfig.GridSizeY;
			sX = (int)worldPos.x % ActiveEditorConfig.GridSizeX;
			if (worldPos.x < 0)
			{
				sX = ActiveEditorConfig.GridSizeX + sX;
			}
			sY = (int)worldPos.y % ActiveEditorConfig.GridSizeY;
			if (worldPos.y < 0)
			{
				sY = ActiveEditorConfig.GridSizeY + sY;
			}
		}
		else
		{
			worldPos -= entityDragOffset;
		}
		// Set entity's position
		EntitySelectedForProperties->SetPosition((float)((int)worldPos.x - sX), (float)((int)worldPos.y - sY));
	}

	// Move view if dragging view
	if (bMouseDrag)
	{		
		sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
		sf::Vector2f delta = sf::Vector2f(pixelPos - lastMousePos);
		SceneView.move(-delta * zoomFactor);
		renderWindowPtr->setView(SceneView);
		lastMousePos = pixelPos;
	}

	// Decrement copy paste timer
	copyPasteTimer -= deltaTime;

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
	Entities.RenderWithDepth(window, ActiveEditorConfig.depthFilterLowerBound, ActiveEditorConfig.depthFilterUpperBound);

	// Render Grid
	if (ActiveEditorConfig.bShowGrid)
	{
		sf::Vector2f topLeftWorld = (*renderWindowPtr).mapPixelToCoords(ActiveEditorConfig.TopLeftPixel);
		sf::Vector2f botRightWorld = (*renderWindowPtr).mapPixelToCoords(ActiveEditorConfig.BotRightPixels);

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

	// Render big grid
	if (ActiveEditorConfig.bShowBigGrid)
	{
		sf::Vector2f topLeftWorld = (*renderWindowPtr).mapPixelToCoords(ActiveEditorConfig.TopLeftPixel);
		sf::Vector2f botRightWorld = (*renderWindowPtr).mapPixelToCoords(ActiveEditorConfig.BotRightPixels);

		int gridRangeX = 6400 / ActiveEditorConfig.BigGridX;
		int gridRangeY = 6400 / ActiveEditorConfig.BigGridY;

		for (int x = -gridRangeX; x < gridRangeX; ++x)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f((float)(x * ActiveEditorConfig.BigGridX), topLeftWorld.y);
			line[0].color = ActiveEditorConfig.BigGridColor;
			line[1].position = sf::Vector2f((float)(x * ActiveEditorConfig.BigGridX), botRightWorld.y);
			line[1].color = ActiveEditorConfig.BigGridColor;
			window.draw(line, 2, sf::Lines);
		}

		for (int y = -gridRangeY; y < gridRangeY; ++y)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f(topLeftWorld.x, (float)(y * ActiveEditorConfig.BigGridY));
			line[0].color = ActiveEditorConfig.BigGridColor;
			line[1].position = sf::Vector2f(botRightWorld.x, (float)(y * ActiveEditorConfig.BigGridY));
			line[1].color = ActiveEditorConfig.BigGridColor;
			window.draw(line, 2, sf::Lines);
		}
	}

	// Show Selection
	if (EntitySelectedForProperties != nullptr)
	{
		float x = EntitySelectedForProperties->GetSprite().getPosition().x;
		float y = EntitySelectedForProperties->GetSprite().getPosition().y;
		float width = (float) EntitySelectedForProperties->GetSprite().getTextureRect().width * EntitySelectedForProperties->GetScale();
		float height = (float) EntitySelectedForProperties->GetSprite().getTextureRect().height * EntitySelectedForProperties->GetScale();

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
		selectionColor.a /= 5;
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

void Scene_Editor::DrawEntityPropertyUI()
{
	ImGui::Begin("Entities and Properties", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f((float) ActiveEditorConfig.BotRightPixels.x, (float) ActiveEditorConfig.TopLeftPixel.y));
	ImGui::SetWindowSize(sf::Vector2f((float) ActiveEditorConfig.WindowSizeX - ActiveEditorConfig.BotRightPixels.x, (float) ActiveEditorConfig.WindowSizeY - ActiveEditorConfig.TopLeftPixel.y));

	ImGui::BeginTabBar("");
	if (ImGui::BeginTabItem("Properties"))
	{
		// Display properties for currently selected entity
		if (EntitySelectedForProperties != nullptr)
		{
#pragma region EntityProperties
			ImGui::Separator();
			ImGui::Text("Entity Properties");
			ImGui::Separator();

			int x = (int) EntitySelectedForProperties->GetPosition().x;
			int y = (int) EntitySelectedForProperties->GetPosition().y;
			ImGui::InputInt("X Position", &x);
			ImGui::InputInt("Y Position", &y);
			EntitySelectedForProperties->SetPosition((float) x, (float) y);

			float angle = EntitySelectedForProperties->GetRotation();
			ImGui::InputFloat("Rotation", &angle);
			EntitySelectedForProperties->SetRotation(angle);

			float scale = EntitySelectedForProperties->GetScale();
			ImGui::InputFloat("Scale", &scale);
			EntitySelectedForProperties->SetScale(scale);

			int depth = EntitySelectedForProperties->GetDepth();
			ImGui::InputInt("Depth", &depth);
			EntitySelectedForProperties->SetDepth(depth);

			bool v = EntitySelectedForProperties->IsVisible();
			ImGui::Checkbox("Visible", &v);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Whether to Render this entity");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			EntitySelectedForProperties->SetVisible(v);

			bool a = EntitySelectedForProperties->IsActive();
			ImGui::Checkbox("Active", &a);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Whether to Update this entity");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			EntitySelectedForProperties->SetActive(a);

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region SpriteSheetProperties
			ImGui::Separator();
			ImGui::Text("SpriteSheet Properties");
			ImGui::Separator();

			// Choose Texture
			if (ImGui::Button("Change Texture"))
			{
				textureDialog.Open();
			}
			textureDialog.Display();
			if (textureDialog.HasSelected())
			{
				if (textureDialog.GetSelected().filename().has_extension())
				{
					size_t pos = textureDialog.GetSelected().string().find("\\Assets\\");
					if (pos < 1000)
					{
						std::string path = textureDialog.GetSelected().string().substr(pos).substr(7);
						EntitySelectedForProperties->SetTexturePathAndLoad(path);
						EntitySelectedForProperties->MatchFrameSizeToTexture();
					}
				}
				textureDialog.ClearSelected();
			}

			int si[2];
			si[0] = EntitySelectedForProperties->SpriteSheet.FrameSize.x;
			si[1] = EntitySelectedForProperties->SpriteSheet.FrameSize.y;

			ImGui::InputInt2("Frame Size", si);
			EntitySelectedForProperties->SpriteSheet.FrameSize.x = si[0];
			EntitySelectedForProperties->SpriteSheet.FrameSize.y = si[1];

			if (ImGui::Button("View Sprite Sheet"))
			{
				bTextureShow = !bTextureShow;
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("View the sprite sheet sliced by the specified frame size.");
				ImGui::TextUnformatted("Choose a frame from the sprite sheet to use as the starting frame of the currently selected animation. Tools aside from selection tool cannot be used while this window is open.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (bTextureShow)
			{
				ImGui::Begin("Currently selected texture", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowPos(sf::Vector2f(ActiveEditorConfig.BotRightPixels.x - 610.f, 50.f));
				ImGui::SetWindowSize(sf::Vector2f(600.f, 600.f));

				ImGui::Text("Clicking on a frame will set that frame as the start frame of the currently selected animation.");
				ImGui::Text("Shift + Clicking on a frame will set that frame as the end frame of the currently selected animation.");

				sf::Vector2f textureBounds = sf::Vector2f(EntitySelectedForProperties->SpriteSheet.Sprite.getTexture()->getSize());
				int numWide = (int) textureBounds.x / ((si[0] > 0) ? si[0] : 1);
				int numTall = (int) textureBounds.y / ((si[1] > 0) ? si[1] : 1);
				float buttonSize = 500.f / numWide < 500.f / numTall ? 500.f / numWide : 500.f / numTall;
				for (int i = 0; i < numTall; ++i)
				{
					for (int j = 0; j < numWide; ++j)
					{
						ImGui::PushID((i * numWide) + j);

						sf::Sprite frame = EntitySelectedForProperties->GetSprite();
						frame.setTextureRect(sf::IntRect(j * si[0], i * si[1], si[0], si[1]));

						if (ImGui::ImageButton(frame, sf::Vector2f(buttonSize, buttonSize)))
						{
							if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
							{
								if (EntitySelectedForProperties->SpriteSheet.SelectedAnimation >= 0)
								{
									// Set num frames
									int index = (i * numWide) + j;
									EntitySelectedForProperties->SpriteSheet.Animations[EntitySelectedForProperties->SpriteSheet.SelectedAnimation].NumFrames
										= index - EntitySelectedForProperties->SpriteSheet.Animations[EntitySelectedForProperties->SpriteSheet.SelectedAnimation].StartFrame + 1;
								}
							}
							else
							{
								if (EntitySelectedForProperties->SpriteSheet.SelectedAnimation >= 0)
								{
									// Set start frame
									int index = (i * numWide) + j;
									EntitySelectedForProperties->SpriteSheet.Animations[EntitySelectedForProperties->SpriteSheet.SelectedAnimation].StartFrame = index;
								}
							}
						}

						if (j != numWide - 1)
						{
							ImGui::SameLine();
						}

						ImGui::PopID();
					}
				}

				ImGui::End();
			}

			ImGui::Checkbox("Loop Animation", &EntitySelectedForProperties->SpriteSheet.bRepeat);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Whether this animation loop or freeze at the last frame.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			int selected = static_cast<int>(EntitySelectedForProperties->SpriteSheet.SelectedAnimation);
			ImGui::SliderInt("Anim Index", &selected, 0, EntitySelectedForProperties->SpriteSheet.Animations.size() - 1);
			EntitySelectedForProperties->SpriteSheet.SelectedAnimation = selected;
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Select Animation Index");
				ImGui::TextUnformatted("Select the index of the animation you want to use.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region AnimationProperties
			ImGui::Separator();
			ImGui::Text("Animations");			
			ImGui::Separator();

			if (ImGui::Button("Add new animation"))
			{
				EntitySelectedForProperties->SpriteSheet.Animations.push_back(FAnimation());
			}

			// display animations
			for (size_t i = 0; i < EntitySelectedForProperties->SpriteSheet.Animations.size(); ++i)
			{
				ImGui::Text(std::to_string(i).c_str());
				ImGui::Indent();
				ImGui::PushID(i);
				
				float dur = EntitySelectedForProperties->SpriteSheet.Animations[i].AnimDuration.asSeconds();
				ImGui::InputFloat("Duration", &dur);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("Duration of this animation in seconds. Set to 0 if you want a static sprite.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				EntitySelectedForProperties->SpriteSheet.Animations[i].AnimDuration = sf::seconds(dur);
				int sframe = static_cast<int>(EntitySelectedForProperties->SpriteSheet.Animations[i].StartFrame);
				ImGui::InputInt("StartFrame", &sframe);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("Index of the frame that the animation starts on.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				EntitySelectedForProperties->SpriteSheet.Animations[i].StartFrame = sframe;
				int numFrames = static_cast<int>(EntitySelectedForProperties->SpriteSheet.Animations[i].NumFrames);
				ImGui::InputInt("# of Frames", &numFrames);				
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("Number of frames for this animation, starting at StartFrame.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				EntitySelectedForProperties->SpriteSheet.Animations[i].NumFrames = numFrames;

				if (ImGui::Button("Delete this animation"))
				{
					std::vector<FAnimation>::iterator nth = EntitySelectedForProperties->SpriteSheet.Animations.begin() + i;
					EntitySelectedForProperties->SpriteSheet.Animations.erase(nth);
					--i;
				}

				ImGui::PopID();
				ImGui::Unindent();
			}

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region CollisionProperties
			ImGui::Separator();
			ImGui::Text("Collision Properties");
			ImGui::Separator();



#pragma endregion
		}

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
		
		// Tags options
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

		// Count of entities in level
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

		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();

	ImGui::End();
}

void Scene_Editor::DrawEntityTypeUI()
{
	ImGui::Begin("Entity Types", nullptr, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(sf::Vector2f(0.f, (float) ActiveEditorConfig.BotRightPixels.y));
	ImGui::SetWindowSize(sf::Vector2f((float) ActiveEditorConfig.BotRightPixels.x, (float) ActiveEditorConfig.WindowSizeY - ActiveEditorConfig.BotRightPixels.y));

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
				currTool = TOOL_PLACE;
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
					currTool = TOOL_PLACE;
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
	ImGui::SetWindowSize(sf::Vector2f((float) ActiveEditorConfig.WindowSizeX, (float) ActiveEditorConfig.TopLeftPixel.y));

	ImGui::PushItemWidth(120.f);
	ImGui::InputText("LEVEL ID", levelID, 30);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Level ID is the name of the level.");
		ImGui::TextUnformatted("Save path in /Assets/Levels/.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	if (ImGui::IsItemActive())
	{
		bTypingLevelID = true;
	}
	else
	{
		bTypingLevelID = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		ADKSaveLoad Saver;
		Saver.SaveScene(levelID, *this);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		ADKSaveLoad Loader;
		Loader.LoadToScene(levelID, *this);
	}
	ImGui::SameLine();

	if (ImGui::Button("Config"))
	{
		bShowConfig = !bShowConfig;
	}
	if (bShowConfig)
	{
		ImGui::Begin("ADK Editor Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(sf::Vector2f((float)ActiveEditorConfig.TopLeftPixel.x, (float)ActiveEditorConfig.TopLeftPixel.y));
		ImGui::SetWindowSize(sf::Vector2f((float)ActiveEditorConfig.BotRightPixels.x - ActiveEditorConfig.TopLeftPixel.x,
			(float)ActiveEditorConfig.BotRightPixels.y - ActiveEditorConfig.TopLeftPixel.y));

		// Big grid
		ImGui::Text("Big Grid: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(90.f);
		ImGui::InputInt("Big Grid X", &ActiveEditorConfig.BigGridX);
		ImGui::SameLine();
		ImGui::InputInt("Big Grid Y", &ActiveEditorConfig.BigGridY);
		ImGui::SameLine();
		ImGui::Checkbox("Show Big Grid", &ActiveEditorConfig.bShowBigGrid);
		ImGui::SameLine();
		ImGui::PushItemWidth(130.f);
		ImColor bigGridCol = MoreColors::SFColorToImColor(ActiveEditorConfig.BigGridColor);
		if (ImGui::ColorEdit4("Big Grid Color", (float*)&bigGridCol, ImGuiColorEditFlags_NoInputs))
		{
			EntitySelectedForCreation = nullptr;
		}
		ActiveEditorConfig.BigGridColor = MoreColors::ImColorToSFColor(bigGridCol);

		// Background color
		

		// Copy-Paste delay

		ImGui::End();
	}

	ImGui::SameLine(370.f);

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
	if (ImGui::ColorEdit4("Grid Color", (float*)&gridCol, ImGuiColorEditFlags_NoInputs))
	{
		EntitySelectedForCreation = nullptr;
	}
	ActiveEditorConfig.GridColor = MoreColors::ImColorToSFColor(gridCol);
	ImGui::SameLine();

	ImColor selCol = MoreColors::SFColorToImColor(ActiveEditorConfig.SelectionColor);
	if (ImGui::ColorEdit4("Select Color", (float*)&selCol, ImGuiColorEditFlags_NoInputs))
	{
		EntitySelectedForCreation = nullptr;
	}
	ActiveEditorConfig.SelectionColor = MoreColors::ImColorToSFColor(selCol);
	ImGui::SameLine(1180.f);

	ImGui::Text("Show Depth From");
	ImGui::SameLine();
	ImGui::PushItemWidth(90.f);
	ImGui::InputInt("To", &ActiveEditorConfig.depthFilterLowerBound);
	ImGui::SameLine();
	ImGui::InputInt("###", &ActiveEditorConfig.depthFilterUpperBound);
	ImGui::SameLine();

	// RESET VIEW button
	if (ImGui::Button("Reset View"))
	{
		InitializeSceneView(*renderWindowPtr);
	}	
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Reset View (A)");
		ImGui::TextUnformatted("Reset the view back to the original view.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	ImGui::End();
}

void Scene_Editor::DrawToolsMenuUI()
{
	ImGui::Begin("Tools Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f(0.f, (float) ActiveEditorConfig.TopLeftPixel.y));
	ImGui::SetWindowSize(sf::Vector2f((float)ActiveEditorConfig.TopLeftPixel.x, 
		(float)ActiveEditorConfig.WindowSizeY - ActiveEditorConfig.TopLeftPixel.y - (ActiveEditorConfig.WindowSizeY - ActiveEditorConfig.BotRightPixels.y)));

	ImGui::Text("TOOLS");
	// Selection tool
	if (ImGui::ImageButton(selectButton, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, currTool == TOOL_SELECTION ? sf::Color::Black : sf::Color::White))
	{
		currTool = TOOL_SELECTION;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Selection Tool (F)");
		ImGui::TextUnformatted("Select an entity from the level viewport. Drag them around to reposition them.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Place tool
	if (ImGui::ImageButton(placeButton, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, currTool == TOOL_PLACE ? sf::Color::Black : sf::Color::White))
	{
		currTool = TOOL_PLACE;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Place Tool (D)");
		ImGui::TextUnformatted("Select an entity type from the Entity Types window and place/create entities inside the level.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Brush tool 
	if (ImGui::ImageButton(brushButton, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, currTool == TOOL_BRUSH ? sf::Color::Black : sf::Color::White))
	{
		currTool = TOOL_BRUSH;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Brush Tool (S)");
		ImGui::TextUnformatted("Select an entity type from the Entity Types window and place/create MULTIPLE entities at a time. Probably a good idea to turn on 'Snap to Grid' for this.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Picker tool
	if (ImGui::ImageButton(pickerButton, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, currTool == TOOL_PICKER ? sf::Color::Black : sf::Color::White))
	{
		currTool = TOOL_PICKER;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Picker Tool (A)");
		ImGui::TextUnformatted("Select an entity type by picking from the level viewport.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	ImGui::End();
}

void Scene_Editor::InitializeSceneView(sf::RenderWindow& window)
{
	SceneView.setViewport(sf::FloatRect(0.03f, 0.04f, 0.78f, 0.738f));
	Scene::InitializeSceneView(window);
	zoomFactor = ViewConfig.Zoom;
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

void Scene_Editor::UpdateEditorConfigWithWindow(sf::RenderWindow& window)
{
	DefaultEditorConfig.WindowSizeX = window.getSize().x;
	DefaultEditorConfig.WindowSizeY = window.getSize().y;
	DefaultEditorConfig.TopLeftPixel.x = (int)(0.03f * window.getSize().x);
	DefaultEditorConfig.TopLeftPixel.y = (int)(0.04f * window.getSize().y);
	DefaultEditorConfig.BotRightPixels.x = (int)(0.78f * window.getSize().x + DefaultEditorConfig.TopLeftPixel.x);
	DefaultEditorConfig.BotRightPixels.y = (int)(0.738f * window.getSize().y + DefaultEditorConfig.TopLeftPixel.y);
	ActiveEditorConfig = DefaultEditorConfig;
	bgRect = sf::FloatRect((float)ActiveEditorConfig.TopLeftPixel.x, (float)ActiveEditorConfig.TopLeftPixel.y,
		(float)ActiveEditorConfig.BotRightPixels.x - ActiveEditorConfig.TopLeftPixel.x, (float)ActiveEditorConfig.BotRightPixels.y - ActiveEditorConfig.TopLeftPixel.y);
}

void Scene_Editor::BrushPlaceHelper()
{
	// Get the current mouse position in the window
	sf::Vector2i pixelPos = sf::Mouse::getPosition(*renderWindowPtr);
	// Convert it to world coordinates
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
		sY = (int)worldPos.y % ActiveEditorConfig.GridSizeY;
		if (worldPos.y < 0)
		{
			sY = ActiveEditorConfig.GridSizeY + sY;
		}
	}

	// Get would-be position of a new entity
	int posX = ((int)worldPos.x) - sX;
	int posY = ((int)worldPos.y) - sY;

	// Check if position already visited (only for brush tool)
	if (currTool == TOOL_BRUSH)
	{
		for (auto pos : BrushVisitedPositions)
		{
			if (pos.x == posX && pos.y == posY)
			{
				// If we've visited this position, then don't place a new entity
				return;
			}
		}
		// Visit this position
		BrushVisitedPositions.push_back(sf::Vector2i(posX, posY));
	}
	
	// Create a new entity
	Entity* created = ADKEditorMetaRegistry::CreateNewEntity(EntitySelectedForCreation->EntityId);
	created->LoadDefaultTexture();
	// Assign it the specific entity id for its entity type
	created->EntityId = EntitySelectedForCreation->EntityId;
	created->SetPosition((float)posX, (float)posY);
	// Add the entity to this scene/level editor's entity list
	Entities.add(created);
	// Set this entity to be selected
	SetEntitySelectedForProperties(created);

	//std::cout << "Placed " << created->EntityId << " at x: " << worldPos.x << "  y: " << worldPos.y << std::endl;
}