#include "TopDownPlayer.h"

TopDownPlayer::TopDownPlayer()
{
	collider = BoxCollider((int) GetPosition().x, (int) GetPosition().y, 10, 10, 64, 64);
}

void TopDownPlayer::Update(float deltaTime)
{
	Entity::Update(deltaTime);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		Move(0.f, -150.f * deltaTime);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		Move(0.f, 150.f * deltaTime);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		Move(-150.f * deltaTime, 0.f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		Move(150.f * deltaTime, 0.f);
	}
}