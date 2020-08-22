#include "Entity.h"
#include "ADKAssets.h"

Entity::Entity()
	: bActive(true)
	, bVisible(true)
	, bCollidable(false)
{
	SetPosition(0.f, 0.f);
	SetRotation(0.f);
	SetScale(1.f);
	SetDepth(0);
}

Entity::Entity(float x, float y)
	: bActive(true)
	, bVisible(true)
	, bCollidable(false)
{
	SetPosition(x, y);
	SetRotation(0.f);
	SetScale(1.f);
	SetDepth(0);
}

Entity::Entity(float x, float y, float inRot, float inScale)
	: bActive(true)
	, bVisible(true)
	, bCollidable(false)
{
	SetPosition(x, y);
	SetRotation(inRot);
	SetScale(inScale);
	SetDepth(0);
}

Entity::~Entity()
{
	// Decrement texture reference count
	ADKAssets::Unload(TexturePath);
}

void Entity::Update(float deltaTime)
{
	// Do animation
	UpdateAnimations(deltaTime);
}

void Entity::Render(sf::RenderTarget& target)
{
	if (bVisible && SpriteSheet.Sprite.getTexture() != nullptr)
	{
		target.draw(SpriteSheet.Sprite);
	}
}

void Entity::DebugRender(sf::RenderTarget& target)
{
	// Debug collider
	sf::RectangleShape col;
	col.setPosition(collider.left, collider.top);
	col.setSize(sf::Vector2f(collider.width, collider.height));
	col.setFillColor(sf::Color(0,0,255,80));
	target.draw(col);
}

void Entity::Move(float x, float y)
{
	SpriteSheet.Sprite.move(x, y);
	collider.setPos(sf::Vector2f(GetPosition()));
}

void Entity::Move(sf::Vector2f delta)
{
	SpriteSheet.Sprite.move(delta);
	collider.setPos(sf::Vector2f(GetPosition()));
}

sf::Vector2f Entity::GetPosition() const
{
	return SpriteSheet.Sprite.getPosition();
}
void Entity::SetPosition(sf::Vector2f newPos)
{
	SpriteSheet.Sprite.setPosition(newPos);
	collider.setPos(sf::Vector2f(GetPosition()));
}
void Entity::SetPosition(float x, float y)
{
	SpriteSheet.Sprite.setPosition(x, y);
	collider.setPos(sf::Vector2f(GetPosition()));
}
float Entity::GetRotation() const
{
	return SpriteSheet.Sprite.getRotation();
}
void Entity::SetRotation(float newRot, bool bAffectCollider) 
{
	SpriteSheet.Sprite.setRotation(newRot);

	if (bAffectCollider)
	{
		sf::FloatRect bounds = SpriteSheet.Sprite.getGlobalBounds();
		collider.setPos(bounds.left, bounds.top);
		collider.width = bounds.width;
		collider.height = bounds.height;
	}
}
float Entity::GetScale() const
{
	return SpriteSheet.Sprite.getScale().x;
}
void Entity::SetScale(float newScale, bool bAffectCollider)
{
	SpriteSheet.Sprite.setScale(newScale, newScale);

	if (bAffectCollider)
	{
		sf::FloatRect bounds = SpriteSheet.Sprite.getGlobalBounds();
		collider.setPos(bounds.left, bounds.top);
		collider.width = bounds.width;
		collider.height = bounds.height;
	}
}
void Entity::SetDepth(int newDepth)
{
	depth = newDepth;
	// TODO MARK ENTITY LIST DEPTH CHANGED
}

void Entity::LoadDefaultTexture()
{
	SetTexturePathAndLoad(TexturePath);
}

void Entity::SetTexturePathAndLoad(const std::string& path, bool forceNoUnload)
{
	// If we already have a texture, then unload that first
	if (SpriteSheet.Sprite.getTexture() != nullptr && forceNoUnload == false)
	{
		ADKAssets::Unload(TexturePath);
	}
	TexturePath = path;
	sf::Texture& LoadedTexture = ADKAssets::Get(TexturePath);
	SpriteSheet.Sprite.setTexture(LoadedTexture, true);

	// If first time setting sprite sheet, set it to entire texture size by default
	if (SpriteSheet.FrameSize.x == 0 && SpriteSheet.FrameSize.y == 0)
	{
		MatchFrameSizeToTexture();
	}
	else
	{
		SpriteSheet.Sprite.setTextureRect(sf::IntRect((int)GetPosition().x, (int)GetPosition().y, SpriteSheet.FrameSize.x, SpriteSheet.FrameSize.y));
	}
}

void Entity::MatchFrameSizeToTexture()
{
	SpriteSheet.FrameSize.x = SpriteSheet.Sprite.getTexture()->getSize().x;
	SpriteSheet.FrameSize.y = SpriteSheet.Sprite.getTexture()->getSize().y;
	SpriteSheet.Sprite.setTextureRect(sf::IntRect((int) GetPosition().x, (int)GetPosition().y, SpriteSheet.FrameSize.x, SpriteSheet.FrameSize.y));
}

sf::Sprite& Entity::GetSprite()
{
	return SpriteSheet.Sprite;
}

void Entity::InitCollider()
{

}

void Entity::Copy(Entity& target, const Entity& source)
{
	target.SpriteSheet = source.SpriteSheet;
	target.EntityId = source.EntityId;
	target.SetTexturePathAndLoad(source.GetTexturePath(), true);
	target.SetActive(source.IsActive());
	target.SetVisible(source.IsVisible());
	target.SetPosition(source.GetPosition());
	target.SetRotation(source.GetRotation());
	target.SetScale(source.GetScale());
	target.SetDepth(source.GetDepth());
	target.GetCollider().offsetX = source.GetColliderCopy().offsetX;
	target.GetCollider().offsetY = source.GetColliderCopy().offsetY;
	target.GetCollider().width = source.GetColliderCopy().width;
	target.GetCollider().height = source.GetColliderCopy().height;
	// TODO copy Tag data
}

void Entity::UpdateAnimations(float deltaTime)
{
#pragma region AnimationUpdate
	if (bVisible && SpriteSheet.Sprite.getTexture() != nullptr && SpriteSheet.Animations.empty() == false)
	{
		// Get num frames wide and tall
		sf::Vector2i textureBounds(SpriteSheet.Sprite.getTexture()->getSize());
		int numFramesWide = textureBounds.x / ((SpriteSheet.FrameSize.x > 0) ? SpriteSheet.FrameSize.x : 1);
		int numFramesTall = textureBounds.y / ((SpriteSheet.FrameSize.y > 0) ? SpriteSheet.FrameSize.y : 1);

		FAnimation currAnim = SpriteSheet.Animations[SpriteSheet.SelectedAnimation];
		int framesInAnim = static_cast<int>(currAnim.NumFrames) < numFramesTall * numFramesWide ? static_cast<int>(currAnim.NumFrames) : numFramesTall * numFramesWide;
		sf::Time timePerFrame = currAnim.AnimDuration / (float)framesInAnim;
		SpriteSheet.ElapsedTime += sf::seconds(deltaTime);

		// Update current frame
		if (timePerFrame.asSeconds() > 0 && currAnim.AnimDuration.asSeconds() > 0.f && currAnim.NumFrames > 0)
		{
			while (SpriteSheet.ElapsedTime > timePerFrame && (SpriteSheet.CurrentFrame - currAnim.StartFrame <= currAnim.NumFrames || SpriteSheet.bRepeat))
			{
				if (SpriteSheet.bRepeat)
				{
					SpriteSheet.CurrentFrame = currAnim.StartFrame + ((SpriteSheet.CurrentFrame - currAnim.StartFrame + 1) % currAnim.NumFrames);
				}
				else
				{
					// Go to next frame if we aren't on the last frame
					if (SpriteSheet.CurrentFrame - currAnim.StartFrame < currAnim.NumFrames - 1)
					{
						++SpriteSheet.CurrentFrame;
					}
				}

				// Check if we went out of bounds
				if (static_cast<int>(SpriteSheet.CurrentFrame) >= numFramesWide * numFramesTall)
				{
					SpriteSheet.CurrentFrame = currAnim.StartFrame;
				}

				// Subtract from elapsed time since last frame
				SpriteSheet.ElapsedTime -= timePerFrame;
			}
		}
		else if (currAnim.AnimDuration.asSeconds() == 0.f)
		{
			// If anim duration == 0, then static sprite at start frame
			SpriteSheet.CurrentFrame = currAnim.StartFrame;
		}

		// by this point current frame will be accurate with frame we need to display
		// Figure out texture rect from current frame
		int xLoc = (SpriteSheet.CurrentFrame % (numFramesWide > 0 ? numFramesWide : 1)) * SpriteSheet.FrameSize.x;
		int yLoc = (SpriteSheet.CurrentFrame / (numFramesWide > 0 ? numFramesWide : 1)) * SpriteSheet.FrameSize.y;
		sf::IntRect frameRect = sf::IntRect(xLoc, yLoc, SpriteSheet.FrameSize.x, SpriteSheet.FrameSize.y);
		// Set new frame to display
		SpriteSheet.Sprite.setTextureRect(frameRect);
	}
#pragma endregion
}