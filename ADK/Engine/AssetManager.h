#pragma once
#include "TextureManager.h"

class AssetManager
{
public:
	AssetManager();

	// --- Asset load methods (context sensitive) --- 
	// Load Texture, give it ID
	void Load(Textures::ID id, const std::string& fileName);

	// --- Asset access methods (context sensitive) ---
	// Get a loaded Texture by ID
	sf::Texture& Get(Textures::ID id);

private:
	TextureManager textureManager;

};

