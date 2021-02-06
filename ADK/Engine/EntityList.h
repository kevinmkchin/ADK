#pragma once

#include <vector>
#include "Entity.h"

/*	probably best to always allocate EntityList dynamically
	*/
class EntityList
{
public:

	EntityList();

	///////////////////////////////////////////////////////////
	// --- ENTITIES ---

	void update(float deltaTime);

	void update_animation_only(float deltaTime);

	// Render the entities of this EntityList
	void render(sf::RenderTarget& target, bool bDebug = false);

	void render_with_depth(sf::RenderTarget& target, int lower, int upper, bool bDebug = false);

	// Render only the vector of entities given in entitiesToRender
	void render_only(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, bool bDebug = false);

	void render_only_with_depth(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, int lower, int upper, bool bDebug = false);

	// Sets the depth changed flag to be true which guarantees the list will be sorted (by depth) at the end of the next update tick (or current update tick if this is called during entity update).
	void mark_depth_changed();

protected:
	// Call after depth of an entity has changed (and mark_depth_changed) to sort entities by depth
	void sortby_render_depth();
	///////////////////////////////////////////////////////////
	
public:

	///////////////////////////////////////////////////////////
	// --- ARRAY ---

	// Adds given entity to list. CheckUnique to ensure addition is unique. Return whether addition was successful.
	bool add(Entity* newEntity, bool bCheckunique = false);

	// Removes given entity from list if it exists. Return whether removal was successful. DOES NOT DELETE OBJECT / DEALLOCATE MEMORY
	bool remove(Entity* entityToRemove);

	// Removes given entity from list if it exists and destroys the entity. Return whether removal was successful.
	bool remove_and_destroy(Entity* entityToRemove);

	// Removes all elements of this list and deletes each of them.
	bool remove_and_destroy_all();

	// Empty the list
	void clear();

	// Get the entity reference at given index
	Entity* at(int index);

	// Find the index of given entity reference
	int find(Entity* entityToFind);

	// Return the number of entities in this entity list
	int size() { return entities.size(); }

	///////////////////////////////////////////////////////////

	//std::vector<Entity*> FindAllWithTag(Tag tag1)
	//std::vector<Entity*> FindAllWithTags(Tag tag1, Tag tag2)
	//std::vector<Entity*> FindAllWithTags(Tag tag1, Tag tag2, Tag tag3)
	//std::vector<Entity*> FindAllWithTags(Tag tag1, Tag tag2, Tag tag3, Tag tag4)

private:

	/* Entities
	In order of depth (deepest/lowest depth value) first, then within same depth, in order of addition.
	*/
	std::vector<Entity*> entities;


	// Whether or not to sort this 
	bool b_depth_changed_flag;

};

// Entity depth comparator
struct DepthComparator
{
	// Compare 2 Entity references by depth
	bool operator ()(const Entity* entity1, const Entity* entity2)
	{
		if (entity1->get_depth() == entity2->get_depth())
			return false;
		return entity1->get_depth() < entity2->get_depth();
	}
};