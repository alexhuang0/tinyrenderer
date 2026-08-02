#pragma once
#include "tgaimage.h"
#include "canvas.h"
#include "geometry/geometry.h"
#include "model.h"

namespace Renderer {
	namespace Colors {
		constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
		constexpr TGAColor green = { 0, 255, 0, 255 };
		constexpr TGAColor red = { 0, 0, 255, 255 };
		constexpr TGAColor blue = { 255, 128, 64, 255 };
		constexpr TGAColor yellow = { 0, 200, 255, 255 };
	}

	void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, const TGAColor& color);
	void line(std::vector<std::array<int, 2>>& horizLineEdges, int maxY, bool isRight
		, int ax, int ay, int bx, int by, TGAImage& framebuffer,
		const TGAColor& color);
	std::tuple<int, int, int> const project(const vec3& vec);
	void drawFaceTBorders(const Model& model, std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color);

	void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz,
		TGAImage& zbuffer, TGAImage& framebuffer, const TGAColor& color);
	void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz,
		std::vector<double>& zbuffer,
		TGAImage& framebuffer, const TGAColor& color);


};