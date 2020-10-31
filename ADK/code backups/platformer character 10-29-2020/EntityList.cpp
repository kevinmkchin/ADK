#include "EntityList.h"

EntityList::EntityList()
	: bDepthChangedFlag(false)
{
}

void EntityList::Update(float deltaTime)
{
	// Update the entities
	for (auto& entity : entities)
	{
		if (entity->IsActive())
		{
			entity->Update(deltaTime);
		}
	}

	// Sort the entities by depth only if MarkDepthChanged
	if (bDepthChangedFlag)
	{
		std::sort(entities.begin(), entities.end(), DepthComparator());
		bDepthChangedFlag = false;
	}
}

void EntityList::Render(sf::RenderTarget& target, bool bDebug /*= false*/)
{
	// Assume sorted
	// Render the entities	
	for (auto& entity : entities)
	{
		if (bDebug == false)
		{
			if (entity->IsVisible())
			{
				entity->Render(target);
			}
		}
		else
		{
			entity->DebugRender(target);
		}
	}
}

void EntityList::RenderWithDepth(sf::RenderTarget& target, int lower, int upper, bool bDebug /*= false*/)
{
	// Assume sorted
	// Render the entities	
	for (auto& entity : entities)
	{
		if (entity->GetDepth() >= lower)
		{
			if (entity->GetDepth() <= upper)
			{
				if (bDebug == false)
				{
					if (entity->IsVisible())
					{
						entity->Render(target);
					}
				}
				else
				{
					entity->DebugRender(target);
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

void EntityList::RenderOnly(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, bool bDebug /*= false*/)
{
	for (auto& entity : entitiesToRender)
	{
		if (bDebug == false)
		{
			if (entity->IsVisible())
			{
				entity->Render(target);
			}
		}
		else
		{
			entity->DebugRender(target);
		}
	}
}

void EntityList::RenderOnlyWithDepth(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, int lower, int upper, bool bDebug /*= false*/)
{
	for (auto& entity : entitiesToRender)
	{
		if (entity->GetDepth() >= lower)
		{
			if (entity->GetDepth() <= upper)
			{
				if (bDebug == false)
				{
					if (entity->IsVisible())
					{
						entity->Render(target);
					}
				}
				else
				{
					entity->DebugRender(target);
				}
			}
			else
			{
				return;
			}
		}
		else
		{
			continue;
		}
	}
}

void EntityList::MarkDepthChanged()
{
	bDepthChangedFlag = true;
}

bool EntityList::add(Entity* newEntity, bool bCheckUnique)
{
	// Check unique
	if (bCheckUnique)
	{
		if (find(newEntity) == -1)
		{
			return false;
		}
	}
	// Add at the end
	entities.push_back(newEntity);
	// MARK DEPTH CHANGED
	return true;
}

bool EntityList::remove(Entity* entityToRemove)
{
	// See if it exists
	std::vector<Entity*>::iterator toRemove = std::find(entities.begin(), entities.end(), entityToRemove);
	int removalIndex = std::distance(entities.begin(), toRemove);
	if (removalIndex == entities.size())
	{
		return false;
	}
	// Erase
	entities.erase(toRemove);
	return true;
}

bool EntityList::removeAndDestroy(Entity* entityToRemove)
{
	bool success = remove(entityToRemove);
	// Destroy
	delete(entityToRemove);
	return success;
}

void EntityList::clear()
{
	entities.clear();
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