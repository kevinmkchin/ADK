#pragma once

#include <stdlib.h>
#include <cmath>
#include <SFML/Graphics.hpp>

namespace ADKMath
{
	/*	get angle between two 2d vectors */
	static float get_angle_between_vectors(sf::Vector2f vec1, sf::Vector2f vec2)
	{
		float numer = vec1.x * vec2.x + vec1.y * vec2.y;
		float det = vec1.x * vec2.y - vec1.y * vec2.x;
		float rad = std::atan2(det, numer);
		float deg = rad * (180.f / 3.14159265f);
		return deg;
	}

	/*	returns rotated vector */
	static sf::Vector2f rotate_vector(sf::Vector2f vec_to_rot, float angle_anticlockwise_degs)
	{
		float rad = angle_anticlockwise_degs * (3.14159265f / 180.f);

		sf::Vector2f out_vec;
		out_vec.x = (cos(rad) * vec_to_rot.x) - (sin(rad) * vec_to_rot.y);
		out_vec.y = (sin(rad) * vec_to_rot.x) + (cos(rad) * vec_to_rot.y);
		return out_vec;
	}

	/*	max inclusive
	*/
	static float rand_float_range(float min, float max)
	{
		float random = ((float)rand()) / (float)RAND_MAX;
		float from_min = (max - min) * random;
		float out = min + from_min;
		return out;
	}

	/*	max inclusive
	*/
	static int rand_int_range(int min, int max)
	{
		return min + (rand() % (max - min + 1));
	}

	/*	Generate true with a [0; 1] probability
	*/
	static bool rand_chance(float percent)
	{
		return rand_float_range(0.f, 1.f) <= percent;
	}

	static bool flip_coin()
	{
		return rand_chance(0.5f);
	}
}
