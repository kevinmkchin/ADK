#pragma once

#include <vector>
#include "Entity.h"

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

class EntityList
{
public:
	void add(Entity* newEntity);

	// Removes given entity from list if it exists DOES NOT DELETE OBJECT / DEALLOCATE MEMORY
	void remove(Entity* entityToRemove);

	// Removes given entity from list if it exists and destroys the entity.
	void removeAndDestroy(Entity* entityToRemove);

	Entity* at(int index);

	int find(Entity* entityToFind);

	//std::vector<Entity*> FindAllWithTag(Tag tag1)
	//std::vector<Entity*> FindAllWithTags(Tag tag1, Tag tag2)
	//std::vector<Entity*> FindAllWithTags(Tag tag1, Tag tag2, Tag tag3)
	//std::vector<Entity*> FindAllWithTags(Tag tag1, Tag tag2, Tag tag3, Tag tag4)

	void Update(float deltaTime);

	void Render(sf::RenderTarget& target);

	//void RenderOnly

	void MarkDepthChanged();

	int size() { return entities.size(); }

private:
	/* Entities
	In order of depth (deepest/lowest depth value) first, then within same depth, in order of addition
	*/
	std::vector<Entity*> entities;
	// Whether or not to sort this 
	bool bDepthChanged;

};