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

	vec4 world_light_; // sun in eye space
	vec3 tri_[3]{}; // triangle in eye space
	std::array<vec3, 3> normals_{}; // tri vertices' normals in eye space
	std::array<vec2, 3> varying_uv_{};

	PhongShader(const Model& m, const RenderContext& rc, const vec4& sun)
		: model_{ m }
		, rContext_{ rc }
	{
		// w = 0. bc its a vector
		vec4 light_4d{ rContext_.ModelView * sun };
		world_light_ = normalize(light_4d);
	}

	vec4 vertex(int iface, int ivert) {
		vec4 v = model_.vert(iface, ivert); // cur vertex in obj coord
		vec4 global_pos = rContext_.ModelView * vec4{ v.x, v.y, v.z, 1. };
		tri_[ivert] = global_pos.xyz();

		normals_[ivert] = (rContext_.ModelView.inverse().transpose() * model_.normal(iface, ivert)).xyz();

		vec2 uvCoord{ model_.uv_coords(iface, ivert) };
		varying_uv_[ivert] = uvCoord;

		return rContext_.Perspective * global_pos;
	}

	std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const override {
		matrix<3, 2> ABC{ varying_uv_ };
		vec2 uv_coords{ bary_coords * ABC };

		matrix<3, 2> tri_edges{ matrix<2, 3>{ tri_[1] - tri_[0], tri_[2] - tri_[0] }.transpose() };
		matrix<2, 2> uv_edges{
			matrix<2, 2>{varying_uv_[1] - varying_uv_[0], varying_uv_[2] - varying_uv_[0]}.transpose()
		};

		matrix<3, 2> tan_bitan{ tri_edges * uv_edges.inverse() };
		vec3 interpolated_normal{ matrix<3, 3>{normals_} *bary_coords };
		matrix<3, 3> tang_space_basis{};
		for (int i : {0, 1, 2}) {
			tang_space_basis[i] = { tan_bitan[i][0], tan_bitan[i][1], interpolated_normal[i] };
		}

		// NORMAL MAP TEXTURE
		vec4 weighted_normal{ normalize(rContext_.ModelView.inverse().transpose() * model_.normal_tex(uv_coords)) };
		// DIFF MAP
		TGAColor diffMap{ model_.diff(uv_coords) };
		// SPEC SHADER
		double specIntensity{ model_.spec(uv_coords) };

		// LIGHTING
		// AMBIENT
		constexpr double ambient{ 0.3 };

		// DIFFUSE
		double diffuse{ (std::max(0., dot(weighted_normal, world_light_))) };

		// SPECULAR
		const vec4 reflected_ray{
			normalize(
			2. * dot(weighted_normal, world_light_) * weighted_normal - world_light_
			)
		};
		double specular{ std::pow(
			std::max(0., reflected_ray.z), // obj points to simply +z axis (in eye coords) since camera @ (0, 0). r.z since (0,0,1) (viewer dir)*(r.x, r.y, r.z) = r.z
			specIntensity
		) };

		TGAColor final_FragColor{ diffMap };
		for (int i : {0, 1, 2}) {
			final_FragColor[i] *= std::min(1., ambient + 0.7 * diffuse + 0.4 * specular);
		}

		return { false, final_FragColor };
	}
};

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
