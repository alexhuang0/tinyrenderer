#include "renderer.h"
#include <array>
#include <omp.h>

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

	int signed_parallelogram_area(int ax, int ay, int bx, int by, int cx, int cy) {
		// AB cross AC
		return (cx - ax) * (by - ay) - ((cy - ay) * (bx - ax));
	}

	void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, const TGAColor& color) {
		int minX{ std::min(ax, std::min(bx, cx)) };
		int minY{ std::min(ay, std::min(by, cy)) };
		int maxX{ std::max(ax, std::max(bx, cx)) };
		int maxY{ std::max(ay, std::max(by, cy)) };
		float area2{ static_cast<float>(signed_parallelogram_area(ax, ay, bx, by, cx, cy)) }; // double of area triangle
		if (area2 > -2) return;

#pragma omp parallel for
		for (int x{ minX }; x <= maxX; ++x) {
			for (int y{ minY }; y <= maxY; ++y) {
				float bcp{ signed_parallelogram_area(x, y, bx, by, cx, cy) / area2 }; // alpha
				float cap{ signed_parallelogram_area(ax, ay, x, y, cx, cy) / area2 }; // beta
				float abp{ 1.0f - bcp - cap }; // gamma

				if (bcp >= 0 && cap >= 0 && abp >= 0) {
					framebuffer.set(x, y, color);
				}
			}
		}

	}
}
