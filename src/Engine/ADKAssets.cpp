#include "ADKAssets.h"

sf::Texture& ADKAssets::get(const std::string& fileName)
{
	return *ADKTextures::get(fileName);
}

void ADKAssets::unload(const std::string& fileName)
{
	ADKTextures::unload(fileName);
}