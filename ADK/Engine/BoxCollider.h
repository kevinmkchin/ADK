#pragma once

#include <SFML/Graphics.hpp>


class BoxCollider : public sf::IntRect
{
public:
	BoxCollider();
	BoxCollider(int InX, int InY, int InWidth, int InHeight);
	BoxCollider(int InX, int InY, int InOffsetX, int InOffsetY, int InWidth, int InHeight);

	void setPos(int x, int y);
	void setPos(sf::Vector2i pos);
	void move(int x, int y);
	void move(sf::Vector2i delta);

public:
	// X offset from position
	int offsetX;
	// Y offset from position
	int offsetY;
	// width of box
	int width;
	// height of box
	int height;

};

