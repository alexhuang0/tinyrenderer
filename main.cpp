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
#include "our_gl.h"
#include "shaders.h"

int main(int argc, char** argv) {
	using namespace Canvas;


	Model model("diablo3_pose");

	RenderContext rContext{};
	rContext.lookat(eye, center, up);
	rContext.init_perspective(norm(eye - center));
	rContext.init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
	rContext.composeTransforms();

	TGAImage framebuffer(width, height, TGAImage::RGB);
	rContext.init_zbuffer();

	const vec4 world_light{ 1, 1, 1, 0 };

	PhongShader phong(model, rContext, world_light);

	for (int f{ 0 }; f < model.nfaces(); ++f) {
		Triangle triang_vertices{
			phong.vertex(f, 0),
			phong.vertex(f, 1),
			phong.vertex(f, 2)
		};

		rContext.rasterize(triang_vertices, phong, framebuffer);
	}

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
