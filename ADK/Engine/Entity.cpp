#include "Entity.h"

Entity::Entity()
	: bActive(true)
	, bVisible(true)
{
	SetPosition(0.f, 0.f);
	SetRotation(0.f);
	SetScale(1.f);
	SetDepth(0);
}

Entity::Entity(float x, float y)
	: bActive(true)
	, bVisible(true)
{
	SetPosition(x, y);
	SetRotation(0.f);
	SetScale(1.f);
	SetDepth(0);
}

Entity::Entity(float x, float y, float inRot, float inScale)
	: bActive(true)
	, bVisible(true)
{
	SetPosition(x, y);
	SetRotation(inRot);
	SetScale(inScale);
	SetDepth(0);
}


void Entity::Update(float deltaTime)
{

}

void Entity::Render(sf::RenderTarget& target)
{
	if (bVisible && sprite.getTexture() != nullptr)
	{
		target.draw(sprite);
	}
}

void Entity::DebugRender()
{

}

sf::Vector2f Entity::GetPosition()
{
	return position;
}
void Entity::SetPosition(sf::Vector2f newPos)
{
	position = newPos;
	sprite.setPosition(position);
}
void Entity::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	sprite.setPosition(position);
}
float Entity::GetRotation()
{
	return rotation;
}
void Entity::SetRotation(float newRot) 
{
	rotation = newRot;
	sprite.setRotation(rotation);
}
float Entity::GetScale() 
{
	return scale;
}
void Entity::SetScale(float newScale)
{
	scale = newScale;
	sprite.setScale(scale, scale);
}
void Entity::SetDepth(int newDepth)
{
	depth = newDepth;
	// TODO MARK ENTITY LIST DEPTH CHANGED
}
Textures::ID Entity::GetTextureId()
{
	return TextureId;
}
void Entity::SetTextureId(Textures::ID newTexId)
{
	TextureId = newTexId;
	// TODO set sprite texture based on new texture id
}
void Entity::SetSpriteTexture(sf::Texture& inTexture)
{
	sprite.setTexture(inTexture);
}
sf::Sprite& Entity::GetSprite()
{
	return sprite;
}