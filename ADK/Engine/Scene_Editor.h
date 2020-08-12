#pragma once
#include "Scene.h"


enum ToolMode
{
	TOOL_SELECTION,		// Select a placed entity
	TOOL_PLACE,			// Place a single entity
	TOOL_BRUSH,			// Place multiple entities
	TOOL_ERASER,
	TOOL_PICKER			// Picks entity type from level
};

struct FEditorConfig
{
public:
	int GridSizeX;
	int GridSizeY;
	sf::Color GridColor;
	bool bShowGrid;
	bool bSnapToGrid;

	int BigGridX;
	int BigGridY;	
	sf::Color BigGridColor;
	bool bShowBigGrid;

	sf::Color SelectionColor;

	sf::Vector2i TopLeftPixel; // Top Left corner of the editor viewport in window pixel space
	sf::Vector2i BotRightPixels; // Bottom Right corner of the editor viewport in window pixel space
	int WindowSizeX;
	int WindowSizeY;

	int depthFilterLowerBound;
	int depthFilterUpperBound;
	// Tag filter?

	// Defaults
	FEditorConfig();
};

class Scene_Editor : public Scene
{

public:
	Scene_Editor();

	virtual void BeginScene(sf::RenderWindow& window) override;

	virtual void EndScene(sf::RenderWindow& window) override;

	virtual void ProcessEvents(sf::Event& event) override;

	virtual void PreUpdate(float deltaTime) override;

	virtual void Update(float deltaTime) override;

	virtual void PostUpdate(float deltaTime) override;

	virtual void PreRender(sf::RenderWindow& window) override;

	virtual void Render(sf::RenderWindow& window) override;

	virtual void PostRender(sf::RenderWindow& window) override;

protected:
	// ImGui
	void DrawEditorUI();

private:
	// ImGui
	void DrawMenuAndOptionsBarUI();
	void DrawToolsMenuUI();
	void DrawEntityTypeUI();
	void DrawEntityPropertyUI();

	// Editor logic
	// USE THIS TO SET ENTITY SELECTED FOR PROPERTIES
	void SetEntitySelectedForProperties(Entity* newSelection);

	// Update editor window positions and size (imgui windows)
	void UpdateEditorConfigWithWindow(sf::RenderWindow& window);

	// Create and place a new entity at the brush location
	void BrushPlaceHelper();

private:
	// Editor Configurations
	FEditorConfig DefaultEditorConfig;
	FEditorConfig ActiveEditorConfig;

	// List of available entities to create
	EntityList EntityTypes;

	// Editor window pointer
	sf::RenderWindow* renderWindowPtr;
	
	// Entity that holds the EntityId of the Entity type to create
	Entity* EntitySelectedForCreation;

	// Entity that is currently selected to edit properties or move around
	Entity* EntitySelectedForProperties;

	// Represents the level viewport area
	sf::FloatRect bgRect;

	// Logic stuff
	bool bBrushEnabled;
	bool bEntityDrag;
	sf::Vector2f entityDragOffset;
	bool bMouseDrag;
	sf::Vector2i lastMousePos;
	float zoomFactor;
	bool bTextureShow;
	ToolMode currTool;

	// Remember visited world positions while using brush so we don't create an entity in the same position
	std::vector<sf::Vector2i> BrushVisitedPositions;

	// Tool buttons textures
	sf::Texture selectButton;
	sf::Texture placeButton;
	sf::Texture brushButton;
	sf::Texture pickerButton;

};

