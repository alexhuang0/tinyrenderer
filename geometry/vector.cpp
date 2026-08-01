#include "vector.h"
#include "matrix.h"
#include <cmath>
#include <numbers>

vec3 rot(vec3 v)
{
	constexpr double PI{ std::numbers::pi };
	constexpr double theta{ PI / 6 };

	const matrix<3, 3> Ry{
		vec3{std::cos(theta), 0., std::sin(theta)},
		{0., 1., 0.},
		{-std::sin(theta), 0., std::cos(theta)}
	};

	return Ry * v;
}

vec3 persp(vec3 v) {
	constexpr double c{ 5. };
	return v * (1 / (1 - v.z / c));
}