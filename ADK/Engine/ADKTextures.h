#pragma once

#include <map>
#include <SFML/Graphics.hpp>

class ADKTextures
{
	
public:
	// Loads a texture if it hasn't already been loaded. Increments reference count.
	static sf::Texture* get(const std::string& path);
	// Decrements reference count. Unload a texture if reference count reaches 0.
	static void unload(const std::string& path);

public:
	static std::map<std::string, uint16_t> reference_count;
	static std::map<std::string, sf::Texture*> texture_pointers;

};
#define DEFINE_ADK_TEXTURES std::map<std::string, uint16_t> ADKTextures::reference_count;std::map<std::string, sf::Texture*> ADKTextures::texture_pointers;