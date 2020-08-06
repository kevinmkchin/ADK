#pragma once
#include <memory>
#include <map>

#include <SFML/Graphics.hpp>

namespace Textures
{
	// TODO: DEFINE ALL UNIQUE TEXTURE ID HERE
	enum ID
	{
		Grass,
		Wall,
		Test
	};
}

class TextureManager
{

public:
	// Load Texture into loadedTextures along with their matching Texture ID
	void Load(Textures::ID id, const std::string& fileName);

	// Get Texture from loadedTextures by Texture ID
	sf::Texture& Get(Textures::ID id);

private:
	// std::map and std::unique_ptr takes care of deallocation
	std::map<Textures::ID, std::unique_ptr<sf::Texture>> loadedTextures;

};

