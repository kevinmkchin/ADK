#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>

namespace ADKMath
{
	static float get_angle_between_vectors(sf::Vector2f vec1, sf::Vector2f vec2)
	{
		float numer = vec1.x * vec2.x + vec1.y * vec2.y;
		float det = vec1.x * vec2.y - vec1.y * vec2.x;
		float rad = std::atan2(det, numer);
		float deg = rad * (180.f / 3.14159264f);
		return deg;
	}
}
