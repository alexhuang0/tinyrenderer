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
	std::array<vec3, 3> normals_{}; // tri vertices' normals in eye space
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

		vec3 norm{ model_.normal(face, vert) };
		normals_[vert] = ((rContext_.ModelView.transpose().inverse()) * vec4 { norm.x, norm.y, norm.z, 0. }).xyz();

		return rContext_.Perspective * global_pos;
	}

	std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const override {
		// SMOOTH SHADING
		matrix<3, 3> ABC{ normals_ };
		vec3 weighted_normal{ normalize(bary_coords * ABC) };

		// AMBIENT
		constexpr TGAColor ambient{ 0.3 * 255 };

		// DIFFUSE
		TGAColor diffuse{ (std::max(0., dot(weighted_normal, world_light_))) * 255 };

		// SPECULAR
		constexpr int shininess{ 35 };
		const vec3 reflected_ray{
			normalize(
			2. * dot(weighted_normal, world_light_) * weighted_normal - world_light_
			)
		};
		TGAColor specular{ std::pow(
			std::max(0., reflected_ray.z), // obj points to simply +z axis (in eye coords) since camera @ (0, 0). r.z since (0,0,1) (viewer dir)*(r.x, r.y, r.z) = r.z
			shininess
		) * 255 };

		TGAColor final_color{ std::min(255., ambient[0] + 0.4 * diffuse[0] + 0.9 * specular[0]) };
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

	const vec3 world_light{ 1, 1, 1 };

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
