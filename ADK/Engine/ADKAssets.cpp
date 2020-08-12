#include "ADKAssets.h"

sf::Texture& ADKAssets::Get(const std::string& fileName)
{
	return *ADKTextures::Get(fileName);
}

void ADKAssets::Unload(const std::string& fileName)
{
	ADKTextures::Unload(fileName);
}