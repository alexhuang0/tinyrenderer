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
	using namespace Canvas;

	TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);
	std::vector<double> depthbuffer(Canvas::width * Canvas::height, -std::numeric_limits<double>::max());

	Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)");

	constexpr vec3    eye{ -1,0,2 }; // camera position
	constexpr vec3 center{ 0,0,0 };  // camera direction
	constexpr vec3     up{ 0,1,0 };  // camera up vector

	const matrix<4, 4> ModelView{ Renderer::modelView(eye, center, up) };
	const matrix<4, 4> Perspective{ Renderer::perspective(norm(eye - center)) };
	const matrix<4, 4> ViewPort{ Renderer::viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8) };

	const matrix<4, 4> ComposeTransforms{ Renderer::composeTransforms(Perspective, ModelView) };

	for (int i{ 0 }; i < model.nfaces(); ++i) {
		vec4 triang_vertices[3]{};

		for (int j{ 0 }; j < 3; ++j) {
			vec3 v{ model.vert(i, j) };
			triang_vertices[j] = ComposeTransforms * vec4{ v.x, v.y, v.z, 1. };
		}

		TGAColor rnd;
		for (int channel{ 0 }; channel < 3; ++channel) rnd[channel] = std::rand() % 255;
		Renderer::rasterize(triang_vertices, ViewPort, depthbuffer, framebuffer, rnd);
	}

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
