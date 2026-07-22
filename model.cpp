#include "model.h"
#include <fstream>
#include <sstream>
#include "canvas.h"

vec3 Model::vert(const std::size_t i) const {
	// 0 <= i < nverts()
	return verts[i];
}
vec3 Model::vert(const std::size_t iface, const std::size_t nthvert) const {
	// 0 <= iface < nfaces(), 0 <= nthvert < 3
	return verts[facet_vrt[iface * 3 + nthvert]];
}

Model::Model(const std::string& filename) {
	std::ifstream objFile(filename);

	if (objFile.fail()) {
		// This will print the exact reason (e.g., "No such file or directory")
		std::perror("File open failed");
	}

	std::string curLine{};
	std::string lineStart{};

	int i{ 0 }; // debugging
	while (std::getline(objFile, curLine)) {
		if (curLine.empty()) {
			continue;
		}

		std::istringstream stream{};
		stream.str(curLine);

		stream >> lineStart;

		if (lineStart == "v") {
			// v 0.608654 -0.568839 -0.416318
			double x{};
			double y{};
			double z{};

			if (std::sscanf(curLine.c_str(), "v %lf %lf %lf", &x, &y, &z) == 3) {
				Model::verts.push_back({ x, y, z });
			}
		}
		else if (lineStart == "f") {
			// f 1193/1240/1193 1180/1227/1180 1179/1226/1179
			std::string vCluster{}; // "v/vt/vn"

			int i1{};
			int i2{};
			int i3{};
			int tmp{};
			if (std::sscanf(curLine.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d"
				, &i1, &tmp, &tmp, &i2, &tmp, &tmp, &i3, &tmp, &tmp) == 9) {
				Model::facet_vrt.push_back(static_cast<size_t>(i1) - 1);
				Model::facet_vrt.push_back(static_cast<size_t>(i2) - 1);
				Model::facet_vrt.push_back(static_cast<size_t>(i3) - 1);
			}
		}
	}

	objFile.close();
}

void Model::line(int ax, int ay, int bx, int by, TGAImage& framebuffer,
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
		if (ierror * 2 > dx) {
			y += ystep;
			ierror -= 2 * dx;
		}
	}
}

void Model::drawFaceTBorders(std::size_t facet_idx, TGAImage& framebuffer, const TGAColor& color) {
	const vec3& v1{ vert(facet_idx, 0) };
	const vec3& v2{ vert(facet_idx, 1) };
	const vec3& v3{ vert(facet_idx, 2) };

	auto [ax, ay] = project(v1);
	auto [bx, by] = project(v2);
	auto [cx, cy] = project(v3);

	line(ax, ay, bx, by, framebuffer, color);
	line(ax, ay, cx, cy, framebuffer, color);
	line(cx, cy, bx, by, framebuffer, color);
}

std::tuple<int, int> const Model::project(const vec3& vec) const {
	return {
		std::round((vec[0] + 1.) * Canvas::midWidth),
		std::round((vec[1] + 1.) * Canvas::midHeight),
	};
}