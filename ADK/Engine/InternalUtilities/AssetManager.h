#pragma once
#include "TextureManager.h"

class AssetManager
{
public:
	AssetManager();

	// Asset load methods (context sensitive)
	void Load(Textures::ID id, const std::string& fileName);

	// Asset access methods (context sensitive)
	sf::Texture& Get(Textures::ID id);

private:
	TextureManager textureManager;

};

