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
	//vector test
	//vec4 v1{ 3, 6, 2, -5 };
	//vec4 v2{ 10, 10, -3, 0 };
	////std::cout << v1 - v2;

	//matrix<4, 4> m4{
	//	vec4{ 2, 1, -3, 2 },
	//	vec4{0, -5, 1, 4},
	//	vec4{1, 0, 2, -6},
	//	vec4{7, 4, -1, 3}
	//};
	//matrix<3, 3> m3{
	//	vec3{0, 1, 5},
	//	vec3{3, -6, 9},
	//	vec3{2, 6, 1}
	//};
	//matrix<3, 3> m32{
	//	vec3{1, -1, 1},
	//	vec3{-6, 1, -1},
	//	vec3{3, 1, 1}
	//};

	//std::cout << m4.determinant() << '\n';
	//std::cout << m3.determinant() << '\n';
	//std::cout << m32.determinant() << '\n';
	//std::cout << m4.adjoint() << '\n';
	//std::cout << m4.inverse() << '\n';


	TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);
	//TGAImage depthbuffer(Canvas::width, Canvas::height, TGAImage::GRAYSCALE);
	std::vector<double> depthbuffer(Canvas::width * Canvas::height);

	//int ax = 17, ay = 4;
	//int bx = 55, by = 39;
	//int cx = 23, cy = 59;

	//Renderer::triangle(ax, ay, 255, bx, by, 255, cx, cy, 255, framebuffer);
	//Renderer::triangle(ax, ay, 0, bx, by, 255, cx, cy, 0, framebuffer);
	//Renderer::triangle(ax, ay, 0, bx, by, 0, cx, cy, 255, framebuffer);

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
	//depthbuffer.write_tga_file("zbuffer.tga");
	return 0;
}
