#pragma once

#include <vector>
#include "Entity.h"

class EntityList
{
public:

	EntityList();

	///////////////////////////////////////////////////////////
	// --- ENTITIES ---

	void Update(float deltaTime);

	void Render(sf::RenderTarget& target, bool bDebug = false);

	void RenderWithDepth(sf::RenderTarget& target, int lower, int upper, bool bDebug = false);

	void RenderOnly(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, bool bDebug = false);

	void RenderOnlyWithDepth(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, int lower, int upper, bool bDebug = false);

	// Sets the depth changed flag to be true which guarantees the list will be sorted (by depth) at the end of the next update tick (or current update tick if this is called during entity update).
	void MarkDepthChanged();

	///////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////
	// --- ARRAY ---

	// Adds given entity to list. CheckUnique to ensure addition is unique. Return whether addition was successful.
	bool add(Entity* newEntity, bool bCheckunique = false);

	// Removes given entity from list if it exists. Return whether removal was successful. DOES NOT DELETE OBJECT / DEALLOCATE MEMORY
	bool remove(Entity* entityToRemove);

	// Removes given entity from list if it exists and destroys the entity. Return whether removal was successful.
	bool removeAndDestroy(Entity* entityToRemove);

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
	bool bDepthChangedFlag;

};

// Entity depth comparator
struct DepthComparator
{
	// Compare 2 Entity references by depth
	bool operator ()(const Entity* entity1, const Entity* entity2)
	{
		if (entity1->GetDepth() == entity2->GetDepth())
			return false;
		return entity1->GetDepth() < entity2->GetDepth();
	}
};