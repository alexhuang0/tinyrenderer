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

	// homogeneous coordinates stuff
	matrix<4, 4> viewport(int x, int y) {
		return { 
			{
				vec4{Canvas::width / 2., 0, 0, x + Canvas::width / 2.}, 
				{0, Canvas::height / 2., 0, y + Canvas::height / 2.}, 
				{0,0,1,0}, 
				{0,0,0,1}
			} 
		};
	}

	matrix<4, 4> perspective(double f) {
		return {
			{
			vec4{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, -1/f, 1}
			}
		};
	}

	std::tuple<int, int, int> const project(const vec3& vec) {
		return {
			std::round((vec.x + 1.) * Canvas::midWidth),
			std::round((vec.y + 1.) * Canvas::midHeight),
			std::round((vec.z + 1.) * 255. / 2),
		};
	}

	void drawFaceTBorders(const Model& model, std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color) {
		const vec3& v1{ model.vert(facet_idx, 0) };
		const vec3& v2{ model.vert(facet_idx, 1) };
		const vec3& v3{ model.vert(facet_idx, 2) };

		auto [ax, ay, az] = project(v1);
		auto [bx, by, bz] = project(v2);
		auto [cx, cy, cz] = project(v3);

		line(ax, ay, bx, by, framebuffer, color);
		line(ax, ay, cx, cy, framebuffer, color);
		line(cx, cy, bx, by, framebuffer, color);
	}

	static int signed_parallelogram_area(int ax, int ay, int bx, int by, int cx, int cy) {
		// AB cross AC
		return (cx - ax) * (by - ay) - ((cy - ay) * (bx - ax));
	}

	void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage& zbuffer, TGAImage& framebuffer, const TGAColor& color) {
		int minX{ std::min(ax, std::min(bx, cx)) };
		int minY{ std::min(ay, std::min(by, cy)) };
		int maxX{ std::max(ax, std::max(bx, cx)) };
		int maxY{ std::max(ay, std::max(by, cy)) };
		float area2{ static_cast<float>(signed_parallelogram_area(ax, ay, bx, by, cx, cy)) }; // double of area triangle
		if (area2 > -2) return;

#pragma omp parallel for
		for (int x{ minX }; x <= maxX; ++x) {
			for (int y{ minY }; y <= maxY; ++y) {
				float alpha{ signed_parallelogram_area(x, y, bx, by, cx, cy) / area2 }; // bcp
				float beta{ signed_parallelogram_area(ax, ay, x, y, cx, cy) / area2 }; // cap
				float gamma{ 1.0f - alpha - beta }; // abp

				if (alpha < 0 || beta < 0 || gamma < 0) continue;

				unsigned char z{ static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz) };
				if (z <= zbuffer.get(x, y)[0]) continue;

				zbuffer.set(x, y, { z });
				framebuffer.set(x, y, color);
			}
		}
	}
	void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz,
		std::vector<double>& zbuffer,
		TGAImage& framebuffer, const TGAColor& color) {

		int minX{ std::max(0, std::min(ax, std::min(bx, cx))) };
		int minY{ std::max(0, std::min(ay, std::min(by, cy))) };
		int maxX{ std::min(Canvas::width - 1, std::max(ax, std::max(bx, cx))) };
		int maxY{ std::min(Canvas::height - 1, std::max(ay, std::max(by, cy))) };
		float area2{ static_cast<float>(signed_parallelogram_area(ax, ay, bx, by, cx, cy)) }; // double of area triangle
		if (area2 > -2) return;

#pragma omp parallel for
		for (int x{ minX }; x <= maxX; ++x) {
			for (int y{ minY }; y <= maxY; ++y) {
				float alpha{ signed_parallelogram_area(x, y, bx, by, cx, cy) / area2 }; // bcp
				float beta{ signed_parallelogram_area(ax, ay, x, y, cx, cy) / area2 }; // cap
				float gamma{ 1.0f - alpha - beta }; // abp

				if (alpha < 0 || beta < 0 || gamma < 0) continue;

				unsigned char z{ static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz) };
				if (y * Canvas::width + x >= Canvas::width * Canvas::height) continue;
				if (z <= zbuffer[static_cast<std::size_t>(y * Canvas::width + x)]) continue;

				zbuffer[static_cast<std::size_t>(y * Canvas::width + x)] = z;
				framebuffer.set(x, y, color);
			}
		}
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