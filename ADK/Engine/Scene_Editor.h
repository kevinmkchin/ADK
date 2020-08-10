#pragma once
#include "Scene.h"


enum ToolMode
{
	TOOL_SELECTION,		// Select a placed entity
	TOOL_PLACE,			// Place a single entity
	TOOL_BRUSH,			// Place multiple entities
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


private:
	// Editor Configurations
	FEditorConfig DefaultEditorConfig;
	FEditorConfig ActiveEditorConfig;

	// List of available entities to create
	EntityList EntityTypes;

	sf::RenderWindow* renderWindowPtr;
	
	Entity* EntitySelectedForCreation;

	Entity* EntitySelectedForProperties;

	sf::FloatRect bgRect;
	// Logic stuff
	bool bEntityDrag;
	bool bMouseDrag;
	sf::Vector2f lastMousePos;
	ToolMode currTool;

};

