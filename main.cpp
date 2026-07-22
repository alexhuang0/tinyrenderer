#include "tgaimage.h"
#include <array>
#include <cmath>
#include <vector>

#include <charconv>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255, 0, 255 };
constexpr TGAColor red = { 0, 0, 255, 255 };
constexpr TGAColor blue = { 255, 128, 64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

namespace Canvas {
	constexpr int width{ 800 };
	constexpr int height{ 800 };
	constexpr float midWidth{ (width - 1) / 2 };
	constexpr float midHeight{ (height - 1) / 2 };
	TGAImage framebuffer(width, height, TGAImage::RGB);
} // namespace Canvas

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
		if (ierror * 2 > dx) {
			y += ystep;
			ierror -= 2 * dx;
		}
	}
}

class Vertex {
	int x_{};
	int y_{};
	double z_{};

public:
	Vertex(int x, int y, double z)
		: x_{ x }
		, y_{ y }
		, z_{ z } {
	}

	Vertex() = default;

	int x() const { return x_; }

	int y() const { return y_; }

	double z() const { return z_; }

	void x(int x) { x_ = x; }

	void y(int y) { y_ = y; }

	void z(double z) { z_ = z; }
};

class Face {
	std::vector<Vertex> vertices_{};

public:
	Face(const std::vector<Vertex>& vertices)
		: vertices_{ vertices } {
	}

	const std::vector<Vertex>& getVertices() const { return vertices_; }

	void setVertices(const std::vector<Vertex>& vertices) {
		vertices_ = vertices;
	}

	void drawFaceBorders() {
		line(vertices_[0].x(), vertices_[0].y(), vertices_[1].x(), vertices_[1].y(),
			Canvas::framebuffer, red);
		line(vertices_[0].x(), vertices_[0].y(), vertices_[2].x(), vertices_[2].y(),
			Canvas::framebuffer, red);
		line(vertices_[2].x(), vertices_[2].y(), vertices_[1].x(), vertices_[1].y(),
			Canvas::framebuffer, red);
	}
};

int convertModelCoordsToWorld(double model, int mid) {
	return std::round(mid * (1 + model));
}

int main(int argc, char** argv) {
	std::ifstream objFile(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)");

	if (!objFile.is_open()) {
		// This will print the exact reason (e.g., "No such file or directory")
		std::perror("File open failed");
	}

	std::vector<Vertex> vertices{};

	std::string curLine{};
	std::string lineStart{};

	int i{ 0 };
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

			stream >> x >> y >> z;

			vertices.push_back({ convertModelCoordsToWorld(x, Canvas::midWidth),
								convertModelCoordsToWorld(y, Canvas::midHeight), z });

		}
		else if (lineStart == "f") {
			// f 1193/1240/1193 1180/1227/1180 1179/1226/1179
			std::string vCluster{}; // "v/vt/vn"
			std::vector<int> vIndices{};
			while (stream >> vCluster) {
				std::istringstream vClusterStream(vCluster);
				std::string token;

				std::getline(vClusterStream, token, '/');
				vIndices.push_back(std::stoi(token) - 1);


			}
			// assumes vertices already read
			Face face({ vertices[vIndices[0]], vertices[vIndices[1]],
						vertices[vIndices[2]] });
			face.drawFaceBorders();
		}
	}

	objFile.close();

	Canvas::framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}