#include <cassert>
#include "ADKTextures.h"

sf::Texture* ADKTextures::get(const std::string& path)
{
	// Check if texture isn't loaded yet
	std::map<std::string, sf::Texture*>::iterator it = texture_pointers.find(path);
	if (it != texture_pointers.end())
	{
		// Assert it exists in ReferenceCount
		std::map<std::string, uint16_t>::iterator refCount = reference_count.find(path);
		assert(refCount != reference_count.end());
		// Increment reference count
		++(refCount->second);

		//std::cout << refCount->second << std::endl;

		// Return texture pointer
		return it->second;
	}
	else
	{
		// Assert it doesn't exist in TexturePointers
		std::map<std::string, sf::Texture*>::iterator texPtr = texture_pointers.find(path);
		assert(texPtr == texture_pointers.end());
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
		reference_count.insert(std::make_pair(path, 1));
		texture_pointers.insert(std::make_pair(path, texture));
		// Return new texture pointer
		return texture;
	}

}

void ADKTextures::unload(const std::string& path)
{
	std::map<std::string, sf::Texture*>::iterator texPtr = texture_pointers.find(path);
	std::map<std::string, uint16_t>::iterator refCount = reference_count.find(path);

	// Assert that path exists in ReferenceCount and TexturePointers (i.e. we've reference counted properly)
	assert(texPtr != texture_pointers.end());
	assert(refCount != reference_count.end());

	// Decrement ReferenceCount
	--(refCount->second);

	//std::cout << refCount->second << std::endl;

	// Check if there are no more references
	if (refCount->second == 0)
	{
		// Deallocate texture
		delete(texPtr->second);
		// Erase texture entry from ReferenceCount and TexturePointers
		texture_pointers.erase(texPtr);
		reference_count.erase(refCount);
	}
}