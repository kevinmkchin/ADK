#pragma once

#include <SFML/Graphics.hpp>

struct BoxCollider
{
	// x position
	float left;
	// y position
	float top;
	// width of box
	float width;
	// height of box
	float height;
	// x offset from left
	float offsetX;
	// y offset from top
	float offsetY;

	// Set top-left position of box collider
	void setPos(float x, float y);
	void setPos(sf::Vector2f pos);
	// Move top-left position of box collider
	void move(float x, float y);
	void move(sf::Vector2f delta);

	// Returns whether or not this box collider is pixel colliding with the other box collider
	bool Intersects(BoxCollider& other);

	// Returns whether or not this 2d vector / point is contained by this box collider
	bool Contains(sf::Vector2f other);

	// Checks 1 pixel next to
	//bool IsTouchingTop();
	//bool IsTouchingBottom();
	//bool IsTouchingLeft();
	//bool IsTouchingRight();

	// Collision resolution
	// Return vector to move in order to not be colliding anymore
	sf::Vector2f ResolveCollisionRectangle(BoxCollider& other);

	// Constructors
	BoxCollider();
	BoxCollider(float InLeft, float InTop, float InWidth, float InHeight);
	BoxCollider(float InLeft, float InTop, float InOffsetX, float InOffsetY, float InWidth, float InHeight);
};

