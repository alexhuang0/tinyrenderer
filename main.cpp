#include <array>
#include <cmath>
#include <vector>

#include <charconv>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>

#include "tgaimage.h"

#include "canvas.h"
#include "model.h"
#include "renderer.h"



int main(int argc, char** argv) {
	TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);
	std::vector<double> depthbuffer(Canvas::width * Canvas::height);

	std::string_view obj{ "african_head" };
	Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)");
	for (int i{ 0 }; i < model.nfaces(); ++i) {
		auto [ax, ay, az] = Renderer::project(model.vert(i, 0));
		auto [bx, by, bz] = Renderer::project(model.vert(i, 1));
		auto [cx, cy, cz] = Renderer::project(model.vert(i, 2));
		TGAColor rnd;
		for (int channel{ 0 }; channel < 3; ++channel) rnd[channel] = std::rand() % 255;
		Renderer::triangle(ax, ay, az, bx, by, bz, cx, cy, cz, depthbuffer, framebuffer, rnd);
	}

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
