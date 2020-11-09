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
		float deg = rad * (180.f / 3.14159265f);
		return deg;
	}

	static sf::Vector2f rotate_vector(sf::Vector2f vec_to_rot, float angle_anticlockwise_degs)
	{
		float rad = angle_anticlockwise_degs * (3.14159265f / 180.f);

		sf::Vector2f out_vec;
		out_vec.x = (cos(rad) * vec_to_rot.x) - (sin(rad) * vec_to_rot.y);
		out_vec.y = (sin(rad) * vec_to_rot.x) + (cos(rad) * vec_to_rot.y);
		return out_vec;
	}
}
