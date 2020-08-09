#include <cassert>
#include "TextureManager.h"

void TextureManager::Load(Textures::ID id)
{
	// Wrap new Texture object in unique pointer
	std::unique_ptr<sf::Texture> texture(new sf::Texture());
	// Get file path
	std::string texturePath = TextureManager::TexturePaths.at(id);
	if (&texturePath != nullptr)
	{		
		// Load from file
		if (texture->loadFromFile(texturePath) == false)
		{
			if (texture->loadFromFile("Assets/adk/t_missing.png") == false)
			{
				throw std::runtime_error("TextureManager::Load - Failed to load " + texturePath);
			}
		}
	}
	else
	{
		throw std::runtime_error("TextureManager::Load - Given texture id enum is not associated with a file path. Register it with a file path in TextureManager.h.");
		if (texture->loadFromFile("Assets/adk/t_missing.png") == false)
		{
			throw std::runtime_error("TextureManager::Load - Failed to load backup default texture.");
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