#pragma once

#include <SFML/Graphics.hpp>
#include "TextureManager.h"

/* KEEP THIS BASE ENTITY CLASS VERY LIGHTWEIGHT */
class Entity
{
public:
	Entity();
	Entity(float x, float y);
	Entity(float x, float y, float inRot, float inScale);

	// --- UPDATE ---
	// Do game logic here, but do not render here. Not called if the Entity is not Active
	virtual void Update(float deltaTime);

	// --- RENDER ---
	// Draw the Entity here. Not called if the Entity is not Visible
	virtual void Render(sf::RenderTarget& target);

	// Draw any debug visuals here. Only called if the console is open, but still called even if the Entity is not Visible
	virtual void DebugRender();
	// --------------

	inline bool IsActive() { return bActive; }
	inline void SetActive(bool Active) { bActive = Active; }
	inline bool IsVisible() { return bVisible; }
	inline void SetVisible(bool Visible) { bVisible = Visible; }
	
	virtual sf::Vector2f GetPosition();
	virtual void SetPosition(sf::Vector2f newPos);
	virtual void SetPosition(float x, float y);
	virtual float GetRotation();
	virtual void SetRotation(float newRot);
	virtual float GetScale();
	virtual void SetScale(float newScale);
	inline int GetDepth() const { return depth; }
	void SetDepth(int newDepth);
	Textures::ID GetTextureId();
	void SetTextureId(Textures::ID newTexId);
	void SetSpriteTexture(sf::Texture& inTexture);
	sf::Sprite& GetSprite();

public:
	// Used only to display entity id in the editor.
	std::string EntityId;

private:
	// Whether to Update this entity
	bool bActive;
	// Whether to Render this entity
	bool bVisible;
	// Position of entity
	sf::Vector2f position;
	// Rotation of entity
	float rotation;
	// Scale of entity
	float scale;
	/*	Depth (e.g. z-value for drawing)
		Use cases:
		- Set different scrolling speed based on z-depth to create a parallax effect
		- zelda-like top down: should probably change based on bottom value of sprite so that sprites are drawn over each other correctly
	*/
	int depth;

	Textures::ID TextureId = Textures::Default;
	// Sprite to render TODO replace this with SpriteSheet and animation system
	sf::Sprite sprite; //272 bytes fucking huge

	// Collider

	// TAGS

};

