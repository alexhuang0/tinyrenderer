#include "renderer.h"

namespace {
	template <typename T>
	void sort3(T& x, T& y, T& z) {
		if (x > y) std::swap(x, y);
		if (y > z) std::swap(y, z);
		if (x > y) std::swap(x, y);
	}
}

namespace Renderer {
	void line(int ax, int ay, int bx, int by, TGAImage& framebuffer,
		TGAColor color) {
		// check for steep vertical
		bool steep{ std::abs(by - ay) > std::abs(bx - ax) };
		if (steep) {
			std::swap(ax, ay);
			std::swap(bx, by);
		}

		if (ax > bx) {
			std::swap(ax, bx);
			std::swap(ay, by);
		}

		int dx{ bx - ax };
		int dy{ std::abs(by - ay) };
		int ierror{ 0 };
		int ystep{ by > ay ? 1 : -1 };
		int y{ ay };

		for (int x = ax; x <= bx; ++x) {
			if (steep) {
				framebuffer.set(y, x, color);
			}
			else {
				framebuffer.set(x, y, color);
			}

			ierror += 2 * dy;
			if (ierror > dx) {
				y += ystep;
				ierror -= 2 * dx;
			}
		}
	}

	std::tuple<int, int> const project(const vec3& vec) {
		return {
			std::round((vec[0] + 1.) * Canvas::midWidth),
			std::round((vec[1] + 1.) * Canvas::midHeight),
		};
	}

	void drawFaceTBorders(const Model& model, std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color) {
		const vec3& v1{ model.vert(facet_idx, 0) };
		const vec3& v2{ model.vert(facet_idx, 1) };
		const vec3& v3{ model.vert(facet_idx, 2) };

		auto [ax, ay] = project(v1);
		auto [bx, by] = project(v2);
		auto [cx, cy] = project(v3);

		line(ax, ay, bx, by, framebuffer, color);
		line(ax, ay, cx, cy, framebuffer, color);
		line(cx, cy, bx, by, framebuffer, color);
	}

	void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
		sort3(ay, by, cy);

		line(ax, ay, bx, by, framebuffer, color);
		line(bx, by, cx, cy, framebuffer, color);
		line(cx, cy, ax, ay, framebuffer, color);
	}

	void scanlineRender(const Model& model, std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color) {
		const vec3& v1{ model.vert(facet_idx, 0) };
		const vec3& v2{ model.vert(facet_idx, 1) };
		const vec3& v3{ model.vert(facet_idx, 2) };

		auto [ax, ay] = project(v1);
		auto [bx, by] = project(v2);
		auto [cx, cy] = project(v3);


	}
}
