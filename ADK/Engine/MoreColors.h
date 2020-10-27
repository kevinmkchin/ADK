#pragma once
#include <imgui.h>

#define		MC_CORNFLOWERBLUE		sf::Color(100, 149, 237)
#define		MC_PRUSSIANBLUE			sf::Color(0, 49, 83)
#define		MC_BABYPINK				sf::Color(255, 153, 204)
#define		MC_SOFTYELLOW			sf::Color(218, 237, 100)
#define		MC_OLIVE				sf::Color(112, 130, 56)
#define		MC_CHARCOAL				sf::Color(54, 69, 79)
#define		MC_MOSTLYBLACKBLUE		sf::Color(23, 29, 34)
#define		MC_SOFTRED				sf::Color(171, 114, 124)

namespace MoreColors
{
	// ImGui ImColor to SFML Color
	static sf::Color ImColorToSFColor(ImColor imColor)
	{
		sf::Color ret;
		ret.r = (sf::Uint8) ((float)(imColor.Value.x) * 255);
		ret.g = (sf::Uint8) ((float)(imColor.Value.y) * 255);
		ret.b = (sf::Uint8) ((float)(imColor.Value.z) * 255);
		ret.a = (sf::Uint8) ((float)(imColor.Value.w) * 255);

		return ret;
	}

	// SFML Color to ImGui ImColor
	static ImColor SFColorToImColor(sf::Color sfColor)
	{
		ImVec4 retCol(sfColor);
		ImColor ret(retCol);
		return ret;
	}
}