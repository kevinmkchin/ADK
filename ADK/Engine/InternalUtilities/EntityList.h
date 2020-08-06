#pragma once

#include <list>
#include "../Entities/Entity.h"

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
	void Add(Entity* newEntity);

	// Removes given entity from list if it exists DOES NOT DELETE OBJECT / DEALLOCATE MEMORY
	void Remove(Entity* entityToRemove);

	//std::list<Entity*> FindAll

	void Update(float deltaTime);

	void Render(sf::RenderTarget& target);

	void MarkDepthChanged();

	//void RenderOnly

private:
	/* Entities
	In order of depth (deepest/lowest depth value) first, then within same depth, in order of addition
	*/
	std::list<Entity*> entities;
	// Whether or not to sort this 
	bool bDepthChanged;

};