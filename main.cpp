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

struct PhongShader : IShader {
	const Model& model_;
	const RenderContext& rContext_;
	vec3 tri_[3]{}; // triangle in eye space
	vec3 world_light_; // sun in eye space

	PhongShader(const Model& m, const RenderContext& rc, const vec3& sun)
		: model_{ m }
		, rContext_{ rc }
	{
		// w = 0. bc its a vector
		vec4 light_4d{ rContext_.ModelView * vec4{sun.x, sun.y, sun.z, 0.} };
		world_light_ = normalize(light_4d.xyz());
	}

	vec4 vertex(int face, int vert) {
		vec3 v = model_.vert(face, vert); // cur vertex in obj coord
		vec4 global_pos = rContext_.ModelView * vec4{ v.x, v.y, v.z, 1. };
		tri_[vert] = global_pos.xyz();
		return rContext_.Perspective * global_pos;
	}

	std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const override {
		// AMBIENT
		constexpr TGAColor ambient{ 100 };

		// DIFFUSE
		const vec3 normal{
			normalize(
				cross3d(tri_[1] - tri_[0], tri_[2] - tri_[0])
				// AB cross AC
			)
		};
		TGAColor diffuse{ (std::max(0., dot(normal, world_light_))) * 255 };

		// SPECULAR
		constexpr int shininess{ 1 };
		const vec3 reflected_ray{
			2. * dot(normal, world_light_) * normal - world_light_
		};
		TGAColor specular{ std::pow(
			std::max(0., dot(normalize(Canvas::eye), reflected_ray)),
			shininess
		) * 255 };

		TGAColor final_color{ (ambient[0] + diffuse[0] + specular[0]) / 3 };
		//TGAColor final_color{ ambient };
		//TGAColor final_color{ diffuse };
		//TGAColor final_color{ specular };

		return { false, final_color };
	}
};

int main(int argc, char** argv) {
	using namespace Canvas;

	Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\african_head\african_head.obj)");

	RenderContext rContext{};
	rContext.lookat(eye, center, up);
	rContext.init_perspective(norm(eye - center));
	rContext.init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
	rContext.composeTransforms();

	TGAImage framebuffer(width, height, TGAImage::GRAYSCALE);
	rContext.init_zbuffer();

	const vec3 world_light{ 1, 0, 1 };

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
