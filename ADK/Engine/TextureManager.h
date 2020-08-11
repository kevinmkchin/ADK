#pragma once

#pragma region ignore

#include <memory>
#include <map>
#include <SFML/Graphics.hpp>

#define REGISTER_TEX(id, path) m[id] = path;

#pragma endregion

namespace Textures
{
	// TODO: DEFINE ALL UNIQUE TEXTURE ID HERE
	enum ID
	{
		Default,
		Ajax,
		Stone,
		Braid
	};
}

#pragma region ignore

class TextureManager
{
public:
	static const std::map<Textures::ID, std::string> TexturePaths;
	static std::map<Textures::ID, std::string> create_map() {
		std::map<Textures::ID, std::string> m;
#pragma endregion

	// TODO: Register all texture IDs with texture paths here
	REGISTER_TEX(Textures::Default, "Assets/adk/t_missing.png")
	REGISTER_TEX(Textures::Ajax, "Assets/ajax.png")
	REGISTER_TEX(Textures::Stone, "Assets/adk/t_missing.png")
	REGISTER_TEX(Textures::Braid, "Assets/braid.png")

#pragma region ignore
		return m;
	}
#pragma endregion

public:
	// Load Texture into loadedTextures along with their matching Texture ID
	void Load(Textures::ID id);

	// Get Texture from loadedTextures by Texture ID
	sf::Texture& Get(Textures::ID id);

	// Unload
	

#pragma region ignore
private:
	// std::map and std::unique_ptr takes care of deallocation
	std::map<Textures::ID, std::unique_ptr<sf::Texture>> loadedTextures;

};
#define REGISTER_TEXTURE_PATHS const std::map<Textures::ID, std::string> TextureManager::TexturePaths = TextureManager::create_map();
#pragma endregion