#include "EntityList.h"


void EntityList::Add(Entity* newEntity)
{
	entities.push_back(newEntity);
	// MARK DEPTH CHANGED
}

//void EntityList::Remove(Entity* entityToRemove)
//{
//	if(entities.)
//}

void EntityList::Update(float deltaTime)
{
	// Update the entities
	for (auto & entity : entities)
	{
		if (entity->IsActive())
		{
			entity->Update(deltaTime);
		}
	}
}

void EntityList::Render(sf::RenderTarget& target)
{
	// Sort the entities by depth ONLY if MarkDepthChanged
	entities.sort(DepthComparator());

	// Render the entities	
	for (auto entity : entities)
	{
		if (entity->IsVisible())
		{
			entity->Render(target);
		}
	}
}

//public void RenderOnly(int matchTags)
//{
//	foreach(var entity in entities)
//		if (entity.Visible && entity.TagCheck(matchTags))
//			entity.Render();
//}