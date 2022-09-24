#pragma once
#include <random>

#include "Vector.h"

class RND
{
	std::random_device rd;
	std::mt19937_64 eng = std::mt19937_64(rd());
	std::uniform_real_distribution<double> dist = std::uniform_real_distribution<double>(0,1);
public:
	RND() = default;

	double random_double01()
	{
		return dist(eng);
	}

	Vector3 random_double01_vec3()
	{
		return {dist(eng), dist(eng), dist(eng)};
	}

};
