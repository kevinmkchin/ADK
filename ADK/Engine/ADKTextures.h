#pragma once

#include <map>
#include <SFML/Graphics.hpp>

class ADKTextures
{
	
public:
	// Loads a texture if it hasn't already been loaded. Increments reference count.
	static sf::Texture* Get(const std::string& path);
	// Decrements reference count. Unload a texture if reference count reaches 0.
	static void Unload(const std::string& path);

public:
	static std::map<std::string, uint16_t> ReferenceCount;
	static std::map<std::string, sf::Texture*> TexturePointers;

};
#define DEFINE_ADK_TEXTURES std::map<std::string, uint16_t> ADKTextures::ReferenceCount;std::map<std::string, sf::Texture*> ADKTextures::TexturePointers;