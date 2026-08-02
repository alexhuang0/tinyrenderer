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

struct RandomShader : IShader {
	const Model& model_;
	const RenderContext rContext_;
	TGAColor color{};
	vec3 tri[3]; // triangle in eye coords

	RandomShader(const Model& m, const RenderContext& rc)
		: model_{ m }
		, rContext_{ rc }
	{
	}

	virtual vec4 vertex(int face, int vert) {
		vec3 v = model_.vert(face, vert); // cur vertex in obj coord
		vec4 global_pos = rContext_.ModelView * vec4{ v.x, v.y, v.z, 1. };
		tri[vert] = global_pos.xyz();
		return rContext_.Perspective * global_pos;
	}

	virtual std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const {



		return { false, color };
	}
};

int main(int argc, char** argv) {
	using namespace Canvas;

	Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)");

	RenderContext rContext{};
	rContext.lookat(eye, center, up);
	rContext.init_perspective(norm(eye - center));
	rContext.init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
	rContext.composeTransforms();

	TGAImage framebuffer(width, height, TGAImage::GRAYSCALE);
	rContext.init_zbuffer();

	RandomShader ambient(model, rContext);
	RandomShader diffuse(model, rContext);

	for (int f{ 0 }; f < model.nfaces(); ++f) {
		ambient.color = { 100 };

		Triangle triang_vertices{
			ambient.vertex(f, 0),
			ambient.vertex(f, 1),
			ambient.vertex(f, 2)
		};

		rContext.rasterize(triang_vertices, ambient, framebuffer);
	}

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
