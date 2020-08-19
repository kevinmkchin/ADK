#include "TopDownPlayer.h"

void TopDownPlayer::Update(float deltaTime)
{
	Entity::Update(deltaTime);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		position.y -= 150.f * deltaTime;
		SpriteSheet.Sprite.setPosition(position);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		position.y += 150.f * deltaTime;
		SpriteSheet.Sprite.setPosition(position);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		position.x -= 150.f * deltaTime;
		SpriteSheet.Sprite.setPosition(position);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		position.x += 150.f * deltaTime;
		SpriteSheet.Sprite.setPosition(position);
	}

}

void TopDownPlayer::Render(sf::RenderTarget& target)
{
	Entity::Render(target);


}
