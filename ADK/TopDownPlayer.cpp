#include "TopDownPlayer.h"

TopDownPlayer::TopDownPlayer()
{
	collider = BoxCollider((int) GetPosition().x, (int) GetPosition().y, 64, 64);
}

void TopDownPlayer::Update(float deltaTime)
{
	Entity::Update(deltaTime);

	// Movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		Move(0.f, -75.f * deltaTime);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		Move(0.f, 75.f * deltaTime);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		Move(-75.f * deltaTime, 0.f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		Move(75.f * deltaTime, 0.f);
	}
}