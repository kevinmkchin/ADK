#pragma once

#include <SFML/Graphics.hpp>

#include "BoxCollider.h"

struct FAnimation
{
public:
	// Number of frames in this animation. maybe replace with end frame?
	std::size_t NumFrames = 1; 
	// Starting frame of this animation
	std::size_t StartFrame = 0;
	// Duration of animation (duration of each frame would then be AnimDuration / NumFrames). 0 AnimDuration means static sprite/animation
	sf::Time AnimDuration = sf::seconds(0.f);
};

struct FSpriteSheet
{
public:
// Expose to editor:
	// Sprite to show. Also holds the texture.
	sf::Sprite Sprite;
	// Size of sprite frame in the texture.
	sf::Vector2i FrameSize = sf::Vector2i(0,0);
	// Repeat anim or no
	bool bRepeat = true;
	// Index of currently selected animation
	std::size_t SelectedAnimation = 0;
	// Animations for this sprite
	std::vector<FAnimation> Animations = { FAnimation() };

// Don't expose to editor;
	// Keeps track of current frame.
	std::size_t CurrentFrame = 0;
	// Keeps track of elapsed time since last frame.
	sf::Time ElapsedTime = sf::seconds(0);
};

/* KEEP THIS BASE ENTITY CLASS LIGHTWEIGHT */
class Entity
{
public:
	Entity();
	Entity(float x, float y);
	Entity(float x, float y, float inRot, float inScale);
	virtual ~Entity();

	///////////////////////////////////////////////////////////
	// --- UPDATE ---

	// Do game logic here, but do not render here. Not called if the Entity is not Active. Handles animation logic.
	virtual void Update(float deltaTime);

protected:
	// Tick animations
	void UpdateAnimations(float deltaTime);

	///////////////////////////////////////////////////////////

public:
	///////////////////////////////////////////////////////////
	// --- RENDER ---

	// Draw the Entity here. Not called if the Entity is not Visible
	virtual void Render(sf::RenderTarget& target);


	// Draw any debug visuals here. Only called if the console is open, but still called even if the Entity is not Visible
	virtual void DebugRender(sf::RenderTarget& target);

	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// --- TRANSFORM ---

	void Move(float x, float y);
	void Move(sf::Vector2f delta);

	sf::Vector2f GetPosition() const;
	void SetPosition(sf::Vector2f newPos);
	void SetPosition(float x, float y);

	float GetRotation() const;
	void SetRotation(float newRot, bool bAffectCollider = false);

	float GetScale() const;
	void SetScale(float newScale, bool bAffectCollider = false);

	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// --- ENTITY ---

	inline bool IsActive() const { return bActive; }
	inline void SetActive(bool Active) { bActive = Active; }

	inline bool IsVisible() const { return bVisible; }
	inline void SetVisible(bool Visible) { bVisible = Visible; }

	inline bool IsCollidable() const { return bCollidable; }
	inline void SetCollidable(bool Collidable) { bCollidable = Collidable; }

	inline int GetDepth() const { return depth; }
	void SetDepth(int newDepth);

	// Return SpriteSheet.Sprite
	sf::Sprite& GetSprite();

	BoxCollider& GetCollider() { return collider; }

	virtual void InitCollider();

	// Entity copier
	static void Copy(Entity& target, const Entity& source);

	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// --- TEXTURES ---

	std::string GetTexturePath() const { return TexturePath; }

	// NEVER call these from virtual constructors. Otherwise, the textures for all base types will also end up being loaded.
	virtual void LoadDefaultTexture();
	void SetTexturePathAndLoad(const std::string& path, bool forceNoUnload = false);

	void MatchFrameSizeToTexture();

	///////////////////////////////////////////////////////////

public:

	// Used only to display entity id in the editor
	std::string EntityId;


	// Contains the sprite and all visual information
	FSpriteSheet SpriteSheet;

protected:

	/* Texture filepath within Assets folder (e.g. "adk/t_missing.png")
	TODO remember to set the correct texture path
	You could also do SetTexturePathAndLoad, but be sure all other essential logic from InitialSpriteSheet will still get done.
	*/
	std::string TexturePath = "adk/t_missing.png";


	/*	Depth (e.g. z-value for drawing)
	Use cases:
	- Set different scrolling speed based on z-depth to create a parallax effect
	- zelda-like top down: should probably change based on bottom value of sprite so that sprites are drawn over each other correctly
	*/
	int depth;


	// Whether to Update this entity
	bool bActive;


	// Whether to Render this entity
	bool bVisible;
	

	// Whether to Collide with this entity
	bool bCollidable;


	// Collider
	BoxCollider collider;

	// TAGS

};

