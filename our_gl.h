#pragma once
#include "tgaimage.h"
#include "geometry/geometry.h"
#include "canvas.h"
#include <array>

struct IShader {
	virtual std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const = 0;
};

using Triangle = std::array<vec4, 3>;

struct RenderContext {
	matrix<4, 4> ModelView{};
	matrix<4, 4> Perspective{};
	matrix<4, 4> Viewport{};
	matrix<4, 4> ComposeTransforms{};
	std::vector<double> zbuffer;

	void lookat(const vec<3>& eye, const vec<3>& center, const vec<3>& up);
	void init_viewport(int x, int y, int w, int h);
	void init_perspective(double f);
	void composeTransforms();
	void init_zbuffer();

	void rasterize(const Triangle& clip, const IShader& shader, TGAImage& framebuffer);
};

