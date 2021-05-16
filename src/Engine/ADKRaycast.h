#pragma once
#include <SFML/Graphics.hpp>

class Entity;
class EntityList;
enum Tags : char;

class ADKRaycast
{

public:
	
	/*  Casts a ray of length len from from to to and returns the point at which the ray hits 
		one of the entities in entities with the tag tag_check. entity_hit_ptr gets set to the
		entity that is hit by the ray. Running time is Theta(n) where n is size of entities. */
	static sf::Vector2f raycast2d(sf::Vector2f from, sf::Vector2f to, float len, 
		EntityList* entities, Entity*& entity_hit_ptr, Tags tag_check[], int tag_check_size = 1);

	

};

