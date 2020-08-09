#include "AssetManager.h"

AssetManager::AssetManager()
{
	textureManager = TextureManager();
}

void AssetManager::Load(Textures::ID id)
{
	textureManager.Load(id);
}

sf::Texture& AssetManager::Get(Textures::ID id)
{
	return textureManager.Get(id);
}