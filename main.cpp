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

#include "canvas.h"
#include "model.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255, 0, 255 };
constexpr TGAColor red = { 0, 0, 255, 255 };
constexpr TGAColor blue = { 255, 128, 64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };


int main(int argc, char** argv) {
	Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)");


	TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);

	for (std::size_t i{ 0 }; i < model.nfaces(); ++i) {
		model.drawFaceTBorders(i, framebuffer, red);
	}

	for (std::size_t i{ 0 }; i < model.nverts(); ++i) {
		const vec3& vert{ model.vert(i) };
		auto [ax, ay] = model.project(vert);
		framebuffer.set(ax, ay, white);
	}

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
