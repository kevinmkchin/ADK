#include "EntityList.h"


void EntityList::add(Entity* newEntity)
{
	entities.push_back(newEntity);
	// MARK DEPTH CHANGED
}

void EntityList::remove(Entity* entityToRemove)
{
	std::vector<Entity*>::iterator toRemove = std::find(entities.begin(), entities.end(), entityToRemove);
	// Erase
	entities.erase(toRemove);
}

void EntityList::removeAndDestroy(Entity* entityToRemove)
{
	remove(entityToRemove);
	// Destroy
	delete(entityToRemove);
}

Entity* EntityList::at(int index)
{
	return entities[index];
}

int EntityList::find(Entity* entityToFind)
{
	std::vector<Entity*>::iterator found = std::find(entities.begin(), entities.end(), entityToFind);
	int index = std::distance(entities.begin(), found);
	if (index == entities.size())
	{
		return -1;
	}
	return index;
}

void EntityList::Update(float deltaTime)
{
	// Update the entities
	for (auto& entity : entities)
	{
		entity->Update(deltaTime);
	}

	// Sort the entities by depth ONLY if MarkDepthChanged
	std::sort(entities.begin(), entities.end(), DepthComparator());
}

void EntityList::Render(sf::RenderTarget& target)
{
	// Assume sorted
	// Render the entities	
	for (auto& entity : entities)
	{
		if (entity->IsVisible())
		{
			entity->Render(target);
		}
	}
}

void EntityList::RenderWithDepth(sf::RenderTarget& target, int lower, int upper)
{
	// Assume sorted
	// Render the entities	
	for (auto& entity : entities)
	{
		if (entity->GetDepth() >= lower)
		{
			if (entity->GetDepth() <= upper)
			{
				if (entity->IsVisible())
				{
					entity->Render(target);
				}
			}
			else
			{
				// Don't go through the rest of the list
				return;
			}
		}
		else
		{
			continue;
		}
	}
}

//public void RenderOnly(int matchTags)
//{
//	foreach(var entity in entities)
//		if (entity.Visible && entity.TagCheck(matchTags))
//			entity.Render();
//}