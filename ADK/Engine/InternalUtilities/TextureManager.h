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
	void Load(Textures::ID id, const std::string& fileName);

	sf::Texture& Get(Textures::ID id);

private:
	std::map<Textures::ID, std::unique_ptr<sf::Texture>> loadedTextures;

};

