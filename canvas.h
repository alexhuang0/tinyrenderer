#pragma once
#include "geometry/vector.h"

namespace Canvas {
	constexpr int width{ 800 };
	constexpr int height{ 800 };
	constexpr float midWidth{ (width - 1) / 2.f };
	constexpr float midHeight{ (height - 1) / 2.f };
	constexpr vec4 world_light{ 1, 1, 1, 0 };
	constexpr vec3    eye{ -1,0,2 }; // camera position
	constexpr vec3 center{ 0,0,0 };  // camera direction
	constexpr vec3     up{ 0,1,0 };  // camera up vector
} // namespace Canvas