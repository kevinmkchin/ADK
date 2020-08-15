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

Entity::~Entity()
{
	// Decrement texture reference count
	ADKAssets::Unload(TexturePath);
}

void Entity::Update(float deltaTime)
{
	// Do animation
#pragma region AnimationUpdate
	if (bVisible && SpriteSheet.Sprite.getTexture() != nullptr && SpriteSheet.Animations.empty() == false)
	{
		// Get num frames wide and tall
		sf::Vector2i textureBounds(SpriteSheet.Sprite.getTexture()->getSize());
		int numFramesWide = textureBounds.x / ((SpriteSheet.FrameSize.x > 0) ? SpriteSheet.FrameSize.x : 1);
		int numFramesTall = textureBounds.y / ((SpriteSheet.FrameSize.y > 0) ? SpriteSheet.FrameSize.y : 1);

		// todo put in some divide by zero checks
		FAnimation currAnim = SpriteSheet.Animations[SpriteSheet.SelectedAnimation];
		int framesInAnim = static_cast<int>(currAnim.NumFrames) < numFramesTall * numFramesWide ? static_cast<int>(currAnim.NumFrames) : numFramesTall * numFramesWide;
		sf::Time timePerFrame = currAnim.AnimDuration / (float) framesInAnim;
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

	// Check if active
	if (bActive == false)
	{
		return;
	}

	// Update logic here
}

void Entity::Render(sf::RenderTarget& target)
{
	if (bVisible && SpriteSheet.Sprite.getTexture() != nullptr)
	{
		target.draw(SpriteSheet.Sprite);
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
	SpriteSheet.Sprite.setPosition(position);
}
void Entity::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	SpriteSheet.Sprite.setPosition(position);
}
float Entity::GetRotation()
{
	return rotation;
}
void Entity::SetRotation(float newRot) 
{
	rotation = newRot;
	SpriteSheet.Sprite.setRotation(rotation);
}
float Entity::GetScale() 
{
	return scale;
}
void Entity::SetScale(float newScale)
{
	scale = newScale;
	SpriteSheet.Sprite.setScale(scale, scale);
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

void Entity::SetTexturePathAndLoad(const std::string& path)
{
	// If we already have a texture, then unload that first
	if (SpriteSheet.Sprite.getTexture() != nullptr)
	{
		ADKAssets::Unload(TexturePath);
	}
	TexturePath = path;
	sf::Texture& LoadedTexture = ADKAssets::Get(TexturePath);
	SpriteSheet.Sprite.setTexture(LoadedTexture, true);

	// If first time setting sprite sheet, set it to entire texture size by default
	if (SpriteSheet.FrameSize.x == 0 && SpriteSheet.FrameSize.y == 0)
	{
		SpriteSheet.FrameSize.x = LoadedTexture.getSize().x;
		SpriteSheet.FrameSize.y = LoadedTexture.getSize().y;
	}
}

void Entity::MatchFrameSizeToTexture()
{
	SpriteSheet.FrameSize.x = SpriteSheet.Sprite.getTexture()->getSize().x;
	SpriteSheet.FrameSize.y = SpriteSheet.Sprite.getTexture()->getSize().y;
}

sf::Sprite& Entity::GetSprite()
{
	return SpriteSheet.Sprite;
}