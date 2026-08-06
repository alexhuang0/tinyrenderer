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
	rContext.init_zbuffer();
	TGAImage framebuffer(width, height, TGAImage::RGB);
	PhongShader phong(model, rContext, world_light);

	RenderContext shContext{};
	shContext.lookat(world_light.xyz(), center, up);
	shContext.init_perspective(norm(world_light.xyz() - center));
	shContext.init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
	shContext.init_zbuffer();

	TGAImage shadowbuffer(width, height, TGAImage::GRAYSCALE);
	ShadowShader shadow(model, rContext, shContext, world_light);
	for (int f{ 0 }; f < model.nfaces(); ++f) {
		Triangle triang_vertices{
			shadow.vertex(f, 0),
			shadow.vertex(f, 1),
			shadow.vertex(f, 2),
		};

		shContext.rasterize(triang_vertices, shadow, shadowbuffer);
	}



	//for (int f{ 0 }; f < model.nfaces(); ++f) {
	//	Triangle triang_vertices{
	//		phong.vertex(f, 0),
	//		phong.vertex(f, 1),
	//		phong.vertex(f, 2)
	//	};

	//	rContext.rasterize(triang_vertices, phong, framebuffer);
	//}

	//framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
