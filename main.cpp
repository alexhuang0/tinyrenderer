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
	//Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)");


	TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);

	Renderer::triangle(7, 45, 35, 100, 45, 60, framebuffer, Renderer::Colors::red);
	Renderer::triangle(120, 35, 90, 5, 45, 110, framebuffer, Renderer::Colors::white);
	Renderer::triangle(115, 83, 80, 90, 85, 120, framebuffer, Renderer::Colors::green);
	Renderer::triangle(3, 3, 15, 3, 7, 30, framebuffer, Renderer::Colors::green);

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
