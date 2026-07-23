#pragma once
#include "tgaimage.h"
#include "canvas.h"
#include "geometry.h"
#include "model.h"

namespace Renderer {
	void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color);
	std::tuple<int, int> const project(const vec3& vec);
	void drawFaceTBorders(const Model& model, std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color);


	void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color);
	void scanlineRender(const Model& model, std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color);
};