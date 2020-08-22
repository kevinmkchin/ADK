#pragma once

#include <SFML/Graphics.hpp>


class BoxCollider
{
public:
	BoxCollider();
	BoxCollider(float InX, float InY, float InWidth, float InHeight);
	BoxCollider(float InX, float InY, float InOffsetX, float InOffsetY, float InWidth, float InHeight);

	void setPos(float x, float y);
	void setPos(sf::Vector2f pos);
	void move(float x, float y);
	void move(sf::Vector2f delta);

	// Returns whether or not this box collider is pixel colliding with the other box collider
	bool Intersects(BoxCollider& other);

	//bool Contains(BoxCollider& other);

	// Collision resolution
	// Return vector to move in order to not be colliding anymore
	sf::Vector2f ResolveCollisionRectangle(BoxCollider& other);

	//// Checks 1 pixel next to
	//bool IsTouchingTop();
	//bool IsTouchingBottom();
	//bool IsTouchingLeft();
	//bool IsTouchingRight();

public:
	// x position
	float left;
	// y position
	float top;
	// width of box
	float width;
	// height of box
	float height;
	// X offset from position
	float offsetX;
	// Y offset from position
	float offsetY;

};

