#pragma once

#include <SFML/Graphics.hpp>
#include "ADKReflection.h"
#include "BoxCollider.h"

class EntityList;

struct FAnimation
{
public:
	// Number of frames in this animation. maybe replace with end frame?
	std::size_t num_frames = 1; 
	// Starting frame of this animation
	std::size_t start_frame = 0;
	// Duration of animation (duration of each frame would then be AnimDuration / NumFrames). 0 AnimDuration means static sprite/animation
	sf::Time anim_duration = sf::seconds(0.f);
};

struct FSpriteSheet
{
public:
// Expose to editor:
	// Sprite to show. Also holds the texture.
	sf::Sprite sprite;
	// Size of sprite frame in the texture.
	sf::Vector2i frame_size = sf::Vector2i(0,0);
	// Repeat anim or no
	bool b_repeat = true;
	// Index of currently selected animation
	std::size_t selected_animation = 0;
	// Animations for this sprite
	std::vector<FAnimation> animations = { FAnimation() };

// Don't expose to editor;
	// Keeps track of current frame.
	std::size_t current_frame = 0;
	// Keeps track of elapsed time since last frame.
	sf::Time elapsed_time = sf::seconds(0);
};

/* KEEP THIS BASE ENTITY CLASS LIGHTWEIGHT */
class Entity : public ADKObject
{
public:
	Entity();
	Entity(float x, float y);
	Entity(float x, float y, float inRot, float inScale);
	virtual ~Entity();

	///////////////////////////////////////////////////////////
	// --- UPDATE ---

	// Do game logic here, but do not render here. Not called if the Entity is not Active. Handles animation logic.
	virtual void update(float deltaTime);

	// Tick animations
	void update_animations(float deltaTime);

protected:
	/*	Gets called on the first ever update call to this entity.
	You could load texture here.
	ALWAYS call Entity::begin_play from overriden begin_play. */
	virtual void begin_play();

	///////////////////////////////////////////////////////////

public:
	///////////////////////////////////////////////////////////
	// --- RENDER ---

	// Draw the Entity here. Not called if the Entity is not Visible
	virtual void render(sf::RenderTarget& target);

	// Draw any debug visuals here. Only called if the console is open, but still called even if the Entity is not Visible
	virtual void render_debug(sf::RenderTarget& target, sf::Color debug_color = sf::Color(0, 0, 255, 80));

	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// --- TRANSFORM ---

	void move(float x, float y);
	void move(sf::Vector2f delta);

	sf::Vector2f get_position() const;
	void set_position(float x, float y);
	void set_position(sf::Vector2f newPos);

	float get_rotation() const;
	virtual void set_rotation(float newRot, bool bAffectCollider = false);

	float get_scale() const;
	void set_scale(float newScale, bool bAffectCollider = false);

	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// --- ENTITY ---

	inline bool is_active() const { return b_active; }
	inline void set_active(bool Active) { b_active = Active; }

	inline bool is_visible() const { return b_visible; }
	inline void set_visible(bool Visible) { b_visible = Visible; }

	inline bool is_collidable() const { return b_collidable; }
	inline void set_collidable(bool Collidable) { b_collidable = Collidable; }

	inline int get_depth() const { return depth; }
	void set_depth(int newDepth);

	// Return SpriteSheet.Sprite
	sf::Sprite& get_sprite();

	BoxCollider& get_collider() { return collider; }
	BoxCollider get_collider_copy() const { return collider; }

	virtual void init_collider();

	virtual void collided(Entity* collided_entity);

	// Entity copier - Entity specific fields will only be copied if the given entities are the same type.
	static void copy(Entity& target, const Entity& source);

	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// --- TEXTURES ---

	std::string get_texture_path() const { return texture_path; }

	// NEVER call these from constructors. Otherwise, the textures for all base types will also end up being loaded.
	virtual void load_default_texture();
	void set_texture_path_and_load(const std::string& path, bool forceNoUnload = false);

	void match_framesize_to_texture();

	///////////////////////////////////////////////////////////

	void set_animation_start_frame(int anim_index, int start_frame);

	sf::Vector2f get_origin() const { return origin_private; }

	bool is_using_origin_for_position() const { return b_use_origin_for_position_private; }

	// Should call from constructor of derived classes.
	virtual void set_frame_size(int x, int y);

	// Should call from constructor of derived classes.
	void set_origin(float x, float y);

	// Should call from constructor of derived classes.
	void set_origin(sf::Vector2f in_origin);

	// Should call from constructor of derived classes.
	void use_origin_for_position(bool b_use);

public:

	// Contains the sprite and all visual information
	FSpriteSheet sprite_sheet;

	// prefab
	char prefab_group[20] = "";
	char prefab_id[20] = "";


public:
	std::vector<EntityList*> get_entity_lists() { return entitylists; }

	// WARNING: This does not add this entity to the given EntityList
	bool add_entity_list(EntityList* new_list);

	bool remove_entity_list(EntityList* list_to_remove);

protected:
	// EntityLists that this entity belongs to.
	std::vector<EntityList*> entitylists;

	/* Texture filepath within Assets folder (e.g. "adk/t_missing.png")
	TODO remember to set the correct texture path
	You could also do SetTexturePathAndLoad, but be sure all other essential logic from InitialSpriteSheet will still get done.
	*/
	std::string texture_path = "adk/t_missing.png";

	BoxCollider collider;	// Collider

	// TAGS


	bool b_begun_play = false;

	bool b_anim_paused = false;

private:

	/*	Depth (e.g. z-value for drawing)
	Use cases:
	- Set different scrolling speed based on z-depth to create a parallax effect
	- zelda-like top down: should probably change based on bottom value of sprite so that sprites are drawn over each other correctly
	*/
	int depth;

	bool b_active;			// Whether to Update this entity
	bool b_visible;			// Whether to Render this entity
	bool b_collidable;		// Whether to Collide with this entity

	///////////////////////////////////////////////////////////
	// --- ORIGIN ---

	/** Local origin of position, rotation, and scale transformations. 
		This vector2f is the origin's offset from the top-left corner of the sprite.
		This entity's rotations are performed around this origin. This entity will scale relative to the origin point.
		This will also be the origin of the entity's position if b_use_origin_for_position is true. */
	sf::Vector2f origin_private;

	/** Whether or not to separate position origin from rotation/scale origin.
		If this is false, position origin will always be top-left corner of sprite. */
	bool b_use_origin_for_position_private;

	///////////////////////////////////////////////////////////
	
	bool b_loaded_texture;
};

