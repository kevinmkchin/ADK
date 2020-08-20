#include "BoxCollider.h"

BoxCollider::BoxCollider()
{
	offsetX = 0;
	offsetY = 0;
	top = -1;
	left = -1;
	width = -1;
	height = -1;
}

BoxCollider::BoxCollider(int InX, int InY, int InWidth, int InHeight)
{
	offsetX = 0;
	offsetY = 0;
	top = InX;
	left = InY;
	width = InWidth;
	height = InHeight;
}

BoxCollider::BoxCollider(int InX, int InY, int InOffsetX, int InOffsetY, int InWidth, int InHeight)
{
	offsetX = InOffsetX;
	offsetY = InOffsetY;
	top = offsetX + InX;
	left = offsetY + InY;
	width = InWidth;
	height = InHeight;
}

void BoxCollider::setPos(int x, int y)
{
	top = offsetX + x;
	left = offsetY + y;
}

void BoxCollider::setPos(sf::Vector2i pos)
{
	top = offsetX + pos.x;
	left = offsetY + pos.y;
}

void BoxCollider::move(int x, int y)
{
	top += x;
	left += y;
}

void BoxCollider::move(sf::Vector2i delta)
{
	top += delta.x;
	left += delta.y;
}
