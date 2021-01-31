#include "EntityList.h"

EntityList::EntityList()
	: b_depth_changed_flag(false)
{
}

void EntityList::update(float deltaTime)
{
	// Update the entities
	for (auto& entity : entities)
	{
		if (entity->is_active())
		{
			entity->update(deltaTime);
		}
		else
		{
			entity->update_animations(deltaTime);
		}
	}

	// Sort the entities by depth only if MarkDepthChanged
	if (b_depth_changed_flag)
	{
		std::sort(entities.begin(), entities.end(), DepthComparator());
		b_depth_changed_flag = false;
	}
}

void EntityList::update_animation_only(float deltaTime)
{
	// Update the entities
	for (auto& entity : entities)
	{
		entity->update_animations(deltaTime);
	}
}

void EntityList::render(sf::RenderTarget& target, bool bDebug /*= false*/)
{
	// Assume sorted
	// Render the entities	
	for (auto& entity : entities)
	{
		if (bDebug == false)
		{
			if (entity->is_visible())
			{
				entity->render(target);
			}
		}
		else
		{
			entity->render_debug(target);
		}
	}
}

void EntityList::render_with_depth(sf::RenderTarget& target, int lower, int upper, bool bDebug /*= false*/)
{
	// Assume sorted
	// Render the entities	
	for (auto& entity : entities)
	{
		if (entity->get_depth() >= lower)
		{
			if (entity->get_depth() <= upper)
			{
				if (bDebug == false)
				{
					if (entity->is_visible())
					{
						entity->render(target);
					}
				}
				else
				{
					entity->render_debug(target);
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

void EntityList::render_only(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, bool bDebug /*= false*/)
{
	for (auto& entity : entitiesToRender)
	{
		if (bDebug == false)
		{
			if (entity->is_visible())
			{
				entity->render(target);
			}
		}
		else
		{
			entity->render_debug(target);
		}
	}
}

void EntityList::render_only_with_depth(sf::RenderTarget& target, std::vector<Entity*> entitiesToRender, int lower, int upper, bool bDebug /*= false*/)
{
	for (auto& entity : entitiesToRender)
	{
		if (entity->get_depth() >= lower)
		{
			if (entity->get_depth() <= upper)
			{
				if (bDebug == false)
				{
					if (entity->is_visible())
					{
						entity->render(target);
					}
				}
				else
				{
					entity->render_debug(target);
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

void EntityList::mark_depth_changed()
{
	b_depth_changed_flag = true;
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
	// Mark depth changed
	mark_depth_changed();
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

bool EntityList::remove_and_destroy(Entity* entityToRemove)
{
	bool success = remove(entityToRemove);
	// Destroy
	delete entityToRemove;
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