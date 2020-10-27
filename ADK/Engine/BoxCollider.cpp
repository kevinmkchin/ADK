#include "BoxCollider.h"
#include <cassert>
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

BoxCollider::BoxCollider(float InLeft, float InTop, float InWidth, float InHeight)
{
	offsetX = 0;
	offsetY = 0;
	left = InLeft;
	top = InTop;
	width = InWidth;
	height = InHeight;
}

BoxCollider::BoxCollider(float InLeft, float InTop, float InOffsetX, float InOffsetY, float InWidth, float InHeight)
{
	offsetX = InOffsetX;
	offsetY = InOffsetY;
	left = offsetX + InLeft;
	top = offsetY + InTop;
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
	float bot = top + height;
	float right = left + width;
	float otherbot = other.top + other.height;
	float otherright = other.left + other.width;

	bool notColliding = bot < other.top || top > otherbot || left > otherright || right < other.left;
	if (notColliding == false)
	{
		return true;
	}
	else
	{
		return false;
	}
	return notColliding == false;
}

bool BoxCollider::Contains(sf::Vector2f other)
{
	float bot = top + height - 1;
	float right = left + width - 1;
	return (left <= other.x && other.x <= right) && (top <= other.y && other.y <= bot);
}

bool BoxCollider::IsTouchingTop(BoxCollider& other, float vel)
{
	float bot = top + height;
	float right = left + width;
	float otherbot = other.top + other.height;
	float otherright = other.left + other.width;

	vel = abs(vel);

	return
		top - vel < otherbot &&
		bot > otherbot &&
		right > other.left &&
		left < otherright;
}

bool BoxCollider::IsTouchingBottom(BoxCollider& other, float vel)
{
	float bot = top + height;
	float right = left + width;
	float otherright = other.left + other.width;

	vel = abs(vel);

	return
		bot + vel > other.top &&
		top < other.top &&
		right > other.left &&
		left < otherright;
}

bool BoxCollider::IsTouchingLeft(BoxCollider& other, float vel)
{
	float bot = top + height;
	float right = left + width;
	float otherbot = other.top + other.height;
	float otherright = other.left + other.width;

	vel = abs(vel);

	return
		left - vel < otherright &&
		right > otherright &&
		bot > other.top &&
		top < otherbot;
}

bool BoxCollider::IsTouchingRight(BoxCollider& other, float vel)
{
	float bot = top + height;
	float right = left + width;
	float otherbot = other.top + other.height;
	float otherright = other.left + other.width;

	vel = abs(vel);

	return
		right + vel > other.left &&
		left < other.left &&
		bot > other.top &&
		top < otherbot;
}

sf::Vector2f BoxCollider::ResolveCollisionRectangle(BoxCollider& other)
{
	// Check we are colliding with it
	if (!Intersects(other))
	{
		return sf::Vector2f(0, 0);
	}

	float bot = top + height;
	float right = left + width;
	float otherbot = other.top + other.height;
	float otherright = other.left + other.width;

	// Find distance to move to each side
	float toTop = other.top - bot;
	float toBot = otherbot - top;
	float toLeft = other.left - right;
	float toRight = otherright - left;

	// Get minimum distance to a side
	float min = std::min({ std::abs(toTop), std::abs(toBot), std::abs(toLeft), std::abs(toRight) });

	if (std::abs(toTop) == min)
	{
		return sf::Vector2f(0.f, toTop);
	}
	else if (std::abs(toBot) == min)
	{
		return sf::Vector2f(0.f, toBot);
	}
	else if (std::abs(toLeft) == min)
	{
		return sf::Vector2f(toLeft, 0.f);
	}
	else
	{
		assert(std::abs(toRight) == min);
		return sf::Vector2f(toRight, 0.f);
	}
}