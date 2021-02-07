#include "ADKRaycast.h"
#include "EntityList.h"
#include "Entity.h"
#include "ADKMath.h"

sf::Vector2f ADKRaycast::raycast2d(sf::Vector2f from, sf::Vector2f to, float len, EntityList* entities, Tags tag_check, Entity*& entity_hit_ptr)
{
	// normalized direction
	sf::Vector2f dir = ADKMath::noramlize_vector(to - from);
	// ray scaled by given length
	sf::Vector2f ray = dir * len;
	// end point of ray
	sf::Vector2f endpoint = from + ray;
	// ray slope
	float dir_m = dir.y / dir.x; 
	// ray b
	float ray_b = from.y - (dir_m * from.x);


	// RAY EQUATION
	// int y = (dir_m * XINPUT) + (from.y - (dir_m * from.x));
	// int x = (YINPUT - (from.y - (dir_m * from.x))) / dir_m;

	std::vector<sf::Vector2f> valid_hits;
	std::vector<float> hit_distances;
	std::vector<Entity*> hit_entities;

	// Check each entity to see if it is within path of ray
	for (int i = 0; i < entities->size(); ++i)
	{
		Entity* e = entities->at(i);

		// Check tag of e, if not tag_check, then skip e
		if (e->has_tag(&tag_check) == false)
		{
			continue;
		}

		BoxCollider col = e->get_collider_copy();
		float left = col.left;
		float right = col.left + col.width;
		float top = col.top;
		float bot = col.top + col.height;

		float shortest_valid_dist = -1.f;
		sf::Vector2f valid_hit;

		// check hit left edge
		float potential_hit_x = left;
		float potential_hit_y = (dir_m * potential_hit_x) + ray_b;
		if (top <= potential_hit_y && potential_hit_y <= bot)
		{
			valid_hit = sf::Vector2f(potential_hit_x, potential_hit_y);
			shortest_valid_dist = ADKMath::magnitude(valid_hit - from);
		}
		// check hit right edge
		potential_hit_x = right;
		potential_hit_y = (dir_m * potential_hit_x) + ray_b;
		if (top <= potential_hit_y && potential_hit_y <= bot)
		{
			sf::Vector2f potential_hit = sf::Vector2f(potential_hit_x, potential_hit_y);
			float potential_valid_dist = ADKMath::magnitude(potential_hit - from);
			if (potential_valid_dist < shortest_valid_dist || shortest_valid_dist < 0.f)
			{
				shortest_valid_dist = potential_valid_dist;
				valid_hit = potential_hit;
			}
		}
		// check hit top edge
		potential_hit_y = top;
		potential_hit_x = (potential_hit_y - ray_b) / dir_m;
		if (left <= potential_hit_x && potential_hit_x <= right)
		{
			sf::Vector2f potential_hit = sf::Vector2f(potential_hit_x, potential_hit_y);
			float potential_valid_dist = ADKMath::magnitude(potential_hit - from);
			if (potential_valid_dist < shortest_valid_dist || shortest_valid_dist < 0.f)
			{
				shortest_valid_dist = potential_valid_dist;
				valid_hit = potential_hit;
			}
		}
		// check hit bot edge
		potential_hit_y = bot;
		potential_hit_x = (potential_hit_y - ray_b) / dir_m;
		if (left <= potential_hit_x && potential_hit_x <= right)
		{
			sf::Vector2f potential_hit = sf::Vector2f(potential_hit_x, potential_hit_y);
			float potential_valid_dist = ADKMath::magnitude(potential_hit - from);
			if (potential_valid_dist < shortest_valid_dist || shortest_valid_dist < 0.f)
			{
				shortest_valid_dist = potential_valid_dist;
				valid_hit = potential_hit;
			}
		}

		// Check if hit distance is within length and that we actually hit something and we are in same direction as 
		if (shortest_valid_dist > 0.f && shortest_valid_dist < len)
		{
			float angle_bw_directions = ADKMath::get_angle_between_vectors(valid_hit - from, ray);
			if (-1.f < angle_bw_directions && angle_bw_directions < 1.f)
			{
				valid_hits.push_back(valid_hit);
				hit_distances.push_back(shortest_valid_dist);
				hit_entities.push_back(e);
			}
		}
	}

	if (valid_hits.size() > 0)
	{
		// YES HIT
		size_t index_of_smallest = 0;
		for (size_t i = 1; i < hit_distances.size(); ++i) // find index of smallest valid hit distance
		{
			if (hit_distances[i] < hit_distances[index_of_smallest])
			{
				index_of_smallest = i;
			}
		}

		entity_hit_ptr = hit_entities[index_of_smallest];
		return valid_hits[index_of_smallest];
	}
	else
	{
		// NO HIT
		entity_hit_ptr = nullptr;
		return from;
	}
}
