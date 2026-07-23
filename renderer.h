#pragma once
#include "tgaimage.h"
#include "canvas.h"
#include "geometry.h"
#include "model.h"


namespace Renderer {
	void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color);
	void drawFaceTBorders(const Model& model, std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color);
	std::tuple<int, int> const project(const vec3& vec);
};