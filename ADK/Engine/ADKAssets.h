#pragma once
#include "ADKTextures.h"

class ADKAssets
{
public:
	// --- Asset load methods (context sensitive) --- 

	// --- Asset access methods (context sensitive) ---
	static sf::Texture& Get(const std::string& fileName);

	static void Unload(const std::string& fileName);
};

