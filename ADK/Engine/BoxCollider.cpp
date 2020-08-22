#include "BoxCollider.h"
#include <iostream>
#include <cstdlib>

BoxCollider::BoxCollider()
{
	offsetX = 0;
	offsetY = 0;
	left = -1;
	top = -1;
	width = -1;
	height = -1;
}

BoxCollider::BoxCollider(float InX, float InY, float InWidth, float InHeight)
{
	offsetX = 0;
	offsetY = 0;
	left = InX;
	top = InY;
	width = InWidth;
	height = InHeight;
}

BoxCollider::BoxCollider(float InX, float InY, float InOffsetX, float InOffsetY, float InWidth, float InHeight)
{
	offsetX = InOffsetX;
	offsetY = InOffsetY;
	left = offsetX + InX;
	top = offsetY + InY;
	width = InWidth;
	height = InHeight;
}

void BoxCollider::setPos(float x, float y)
{
	left = offsetX + x;
	top = offsetY + y;
}

void BoxCollider::setPos(sf::Vector2f pos)
{
	left = offsetX + pos.x;
	top = offsetY + pos.y;
}

void BoxCollider::move(float x, float y)
{
	left += x;
	top += y;
}

void BoxCollider::move(sf::Vector2f delta)
{
	left += delta.x;
	top += delta.y;
}

bool BoxCollider::Intersects(BoxCollider& other)
{
	// Pixel perfect
	int bot = top + height - 1;
	int right = left + width - 1;
	int otherbot = other.top + other.height - 1;
	int otherright = other.left + other.width - 1;

	bool notColliding = bot < other.top || top > otherbot || left > otherright || right < other.left;
	return notColliding == false;
}

sf::Vector2f BoxCollider::ResolveCollisionRectangle(BoxCollider& other)
{
	// Check we are colliding with it
	if (!Intersects(other))
	{
		return sf::Vector2f(0, 0);
	}

	float bot = top + height - 1;
	float right = left + width - 1;
	float otherbot = other.top + other.height - 1;
	float otherright = other.left + other.width - 1;

	// Find distance to move to each side
	float toTop = other.top - bot - 1;
	float toBot = otherbot - top + 1;
	float toLeft = other.left - right - 1;
	float toRight = otherright - left + 1;

	// Get minimum distance to a side
	float min = std::min({ std::abs(toTop), std::abs(toBot), std::abs(toLeft), std::abs(toRight) });

	if (std::abs(toTop) == min)
	{
		return sf::Vector2f(0.f, (float)toTop);
	}
	else if (std::abs(toBot) == min)
	{
		return sf::Vector2f(0.f, (float)toBot);
	}
	else if (std::abs(toLeft) == min)
	{
		return sf::Vector2f((float)toLeft, 0.f);
	}
	else if (std::abs(toRight) == min)
	{
		return sf::Vector2f((float)toRight, 0.f);
	}
}