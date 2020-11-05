#pragma once
#include "ADKTextures.h"

class ADKAssets
{
public:
	// --- Asset load methods (context sensitive) --- 

	// --- Asset access methods (context sensitive) ---
	static sf::Texture& get(const std::string& fileName);

	static void unload(const std::string& fileName);
};

