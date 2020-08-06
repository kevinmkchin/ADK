#include <cassert>
#include "TextureManager.h"

void TextureManager::Load(Textures::ID id, const std::string& fileName)
{
	// Wrap new Texture object in unique pointer
	std::unique_ptr<sf::Texture> texture(new sf::Texture());
	// Load from file
	if (texture->loadFromFile(fileName) == false)
	{
		if (texture->loadFromFile("Assets/adk/t_missing.png") == false)
		{
			throw std::runtime_error("TextureManager::Load - Failed to load " + fileName);
		}
	}
	
	auto inserted = loadedTextures.insert(std::make_pair(id, std::move(texture)));
	assert(inserted.second); 
}

sf::Texture& TextureManager::Get(Textures::ID id)
{
	auto found = loadedTextures.find(id);
	assert(found->second);

	return *(found->second);
}