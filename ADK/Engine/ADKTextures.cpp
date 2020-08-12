#include <cassert>
#include "ADKTextures.h"

sf::Texture* ADKTextures::Get(const std::string& path)
{
	// Check if texture isn't loaded yet
	std::map<std::string, sf::Texture*>::iterator it = TexturePointers.find(path);
	if (it != TexturePointers.end())
	{
		// Assert it exists in ReferenceCount
		std::map<std::string, uint16_t>::iterator refCount = ReferenceCount.find(path);
		assert(refCount != ReferenceCount.end());
		// Increment reference count
		++(refCount->second);

		//std::cout << refCount->second << std::endl;

		// Return texture pointer
		return it->second;
	}
	else
	{
		// Assert it doesn't exist in TexturePointers
		std::map<std::string, sf::Texture*>::iterator texPtr = TexturePointers.find(path);
		assert(texPtr == TexturePointers.end());
		// Create new texture and load from file
		sf::Texture* texture = new sf::Texture();
		std::string loadPath = "Assets/";
		loadPath.append(path);
		if (texture->loadFromFile(loadPath) == false)
		{
			if (texture->loadFromFile("Assets/adk/t_missing.png") == false)
			{
				throw std::runtime_error("TextureManager::Load - Failed to load Assets/" + path);
				delete texture;
			}
		}
		// Add to ReferenceCount and TexturePointers
		ReferenceCount.insert(std::make_pair(path, 1));
		TexturePointers.insert(std::make_pair(path, texture));
		// Return new texture pointer
		return texture;
	}

}

void ADKTextures::Unload(const std::string& path)
{
	std::map<std::string, sf::Texture*>::iterator texPtr = TexturePointers.find(path);
	std::map<std::string, uint16_t>::iterator refCount = ReferenceCount.find(path);

	// Assert that path exists in ReferenceCount and TexturePointers (i.e. we've reference counted properly)
	assert(texPtr != TexturePointers.end());
	assert(refCount != ReferenceCount.end());

	// Decrement ReferenceCount
	--(refCount->second);

	//std::cout << refCount->second << std::endl;

	// Check if there are no more references
	if (refCount->second == 0)
	{
		// Deallocate texture
		delete(texPtr->second);
		// Erase texture entry from ReferenceCount and TexturePointers
		TexturePointers.erase(texPtr);
		ReferenceCount.erase(refCount);
	}
}