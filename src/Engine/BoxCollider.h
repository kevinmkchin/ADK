#pragma once

#include <SFML/Graphics.hpp>

struct BoxCollider
{
	float left;		// x position + x offset
	float top;		// y position + y offset
	float width;	// width of box
	float height;	// height of box
	// Use set_offsets instead of setting value directly. Otherwise left and top don't get updated until the next set_pos call.
	float offset_x;	// x offset to an input position. Use set_offsets instead of setting value directly.
	float offset_y;	// y offset to an input position. Use set_offsets instead of setting value directly.

	// Set top-left position of box collider
	void set_pos(float x, float y);
	void set_pos(sf::Vector2f pos);

	// Rotate box collider by by around the origin. Can only rotate angles multiple of 90.
	void rotate(float by, sf::Vector2f origin);

	// Use this instead of 
	void set_offsets(float x_offset, float y_offset);

	// Move top-left position of box collider
	void move(float x, float y);
	void move(sf::Vector2f delta);

	// Returns whether or not this box collider is pixel colliding with the other box collider
	virtual bool intersects(BoxCollider& other);

	// Returns whether or not this 2d vector / point is contained by this box collider
	virtual bool contains(sf::Vector2f other);

	// Checks if touching after move by vel
	bool will_touch_top(BoxCollider& other, float vel);
	bool will_touch_bottom(BoxCollider& other, float vel);
	bool will_touch_left(BoxCollider& other, float vel);
	bool will_touch_right(BoxCollider& other, float vel);

	// Collision resolution
	// Return vector to move in order to not be colliding anymore
	virtual sf::Vector2f resolve_collision_rect(BoxCollider& other);

	// Constructors
	BoxCollider();
	BoxCollider(float in_left, float in_top, float in_width, float in_height);
	BoxCollider(float in_left, float in_top, float in_offset_x, float in_offset_y, float in_width, float in_height);
};