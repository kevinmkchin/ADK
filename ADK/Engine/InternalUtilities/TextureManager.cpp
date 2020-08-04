#include "TextureManager.h"

void TextureManager::Load(Textures::ID id, const std::string& fileName)
{
	// Wrap new Texture object in unique pointer
	std::unique_ptr<sf::Texture> texture(new sf::Texture());
	// Load from file
	texture->loadFromFile(fileName);

	loadedTextures.insert(std::make_pair(id, std::move(texture)));
}

sf::Texture& TextureManager::Get(Textures::ID id)
{
	auto found = loadedTextures.find(id);
	return *(found->second);
}