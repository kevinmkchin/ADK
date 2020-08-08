#pragma once
#include "Scene.h"
class Scene_Editor : public Scene
{

public:
	virtual void BeginScene(sf::RenderTarget& target) override;

	//virtual void EndScene(sf::RenderTarget& target) override;

	virtual void PreUpdate(float deltaTime) override;

	virtual void Update(float deltaTime) override;

	virtual void PostUpdate(float deltaTime) override;

	virtual void PreRender(sf::RenderTarget& target) override;

	virtual void Render(sf::RenderTarget& target) override;

	virtual void PostRender(sf::RenderTarget& target) override;

private:
	std::vector<Entity*> EntityTypes;

};

