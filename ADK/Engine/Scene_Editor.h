#pragma once
#include "Scene.h"
class Scene_Editor : public Scene
{

public:
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
	void DrawMenuAndOptionsBarUI();

	void DrawToolsMenuUI();

	void DrawEntityTypeUI();

	void DrawEntityPropertyUI();

private:
	// List of available entities to create
	EntityList EntityTypes;

	sf::RenderWindow* renderWindowPtr;

	sf::RectangleShape background;

	Entity* EntitySelectedForCreation;
	Entity* EntitySelectedForProperties;

};

