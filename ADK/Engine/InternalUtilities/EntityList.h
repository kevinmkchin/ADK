#pragma once

#include <list>
#include "../Entities/Entity.h"

struct DepthComparator
{
	// Compare 2 Entity references by depth
	bool operator ()(const Entity* entity1, const Entity* entity2)
	{
		if (entity1->GetDepth() == entity2->GetDepth())
			return true;
		return entity1->GetDepth() < entity2->GetDepth();
	}
};

class EntityList
{
public:
	void Add(Entity* newEntity);

	//void Remove();

	//std::list<Entity*> FindAll

	void Update(float deltaTime);

	void Render(sf::RenderTarget& target);

	void MarkDepthChanged();

	//void RenderOnly

private:
	// Entities
	std::list<Entity*> entities;
	// Whether or not to sort this 
	bool bDepthChanged;

};