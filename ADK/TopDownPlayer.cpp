#include "TopDownPlayer.h"

TopDownPlayer::TopDownPlayer()
{
	collider = BoxCollider(GetPosition().x, GetPosition().y, 64.f, 64.f);
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