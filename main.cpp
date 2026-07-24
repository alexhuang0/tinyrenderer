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
#include "renderer.h"



int main(int argc, char** argv) {
	Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\african_head\african_head.obj)");

	TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);

	for (int i{ 0 }; i < model.nfaces(); ++i) {
		auto [ax, ay] = Renderer::project(model.vert(i, 0));
		auto [bx, by] = Renderer::project(model.vert(i, 1));
		auto [cx, cy] = Renderer::project(model.vert(i, 2));
		TGAColor rnd;
		for (int channel{ 0 }; channel < 3; ++channel) rnd[channel] = std::rand() % 255;
		Renderer::triangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
	}

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
