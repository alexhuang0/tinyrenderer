#include "renderer.h"
#include <array>

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
		const TGAColor& color) {
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

	// adds the x coord to horizLineEdges (side effect)
	void line(std::vector<std::array<int, 2>>& horizLineEdges, int maxY, bool isRight
		, int ax, int ay, int bx, int by, TGAImage& framebuffer,
		const TGAColor& color) {
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

		for (int x = ax, i = 0; x <= bx; ++x) {
			if (steep) {
				framebuffer.set(y, x, color);
				horizLineEdges[static_cast<size_t>(maxY - x)][isRight] = y;
			}
			else {
				framebuffer.set(x, y, color);
				horizLineEdges[static_cast<size_t>(maxY - y)][isRight] = x;
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
		// sort ascending
		if (ay > by) { std::swap(ay, by); std::swap(ax, bx); }
		if (by > cy) { std::swap(by, cy); std::swap(bx, cx); }
		if (ay > by) { std::swap(ay, by); std::swap(ax, bx); }

		const int height{ cy - ay + 1 };

		if (ay != by) {
			double x1(ax), x2(ax);
			double slope1{ static_cast<double>(cx - ax) / (cy - ay) }, slope2{ static_cast<double>(bx - ax) / (by - ay) };
			for (int y{ ay }; y <= by; ++y) {
				for (double x{ std::min(x1, x2) }; x < std::max(x1, x2); ++x) {
					framebuffer.set(x, y, color);
				}

				x1 += slope1;
				x2 += slope2;
			}
		}

		if (by != cy) {
			double x1(cx), x2(cx);
			double slope1{ static_cast<double>(ax - cx) / (ay - cy) }, slope2{ static_cast<double>(bx - cx) / (by - cy) };
			for (int y{ cy }; y > by; --y) {
				for (double x{ std::min(x1, x2) }; x < std::max(x1, x2); ++x) {
					framebuffer.set(x, y, color);
				}

				x1 -= slope1;
				x2 -= slope2;
			}
		}
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
