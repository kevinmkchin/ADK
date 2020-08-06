#include "AssetManager.h"

AssetManager::AssetManager()
{
	textureManager = TextureManager();
}

void AssetManager::Load(Textures::ID id, const std::string& fileName)
{
	textureManager.Load(id, fileName);
}

sf::Texture& AssetManager::Get(Textures::ID id)
{
	return textureManager.Get(id);
}