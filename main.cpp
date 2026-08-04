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
	vec4 tri_[3]{}; // triangle in eye space
	std::array<vec4, 3> varying_nrms_{}; // tri vertices' normals in eye space
	std::array<vec2, 3> varying_uv_{}; // tri uv coordinates, written by vertex shader, read by fragment shader

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
		vec4 global_pos = rContext_.ModelView * v;

		tri_[ivert] = global_pos;

		varying_nrms_[ivert] = rContext_.ModelView.inverse().transpose() * model_.normal(iface, ivert);

		vec2 uvCoord{ model_.uv_coords(iface, ivert) };
		varying_uv_[ivert] = uvCoord;

		return rContext_.Perspective * global_pos;
	}

	std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const override {
		matrix<3, 2> ABC{ varying_uv_ };
		vec2 uv_coords{ bary_coords * ABC };

		matrix<2, 4 > tri_edges{ tri_[1] - tri_[0], tri_[2] - tri_[0] };
		matrix<2, 2> uv_edges{ varying_uv_[1] - varying_uv_[0], varying_uv_[2] - varying_uv_[0] };

		matrix<2, 4> tan_bitan{ uv_edges.inverse() * tri_edges };
		vec4 weighted_normal{ normalize(bary_coords * matrix<3, 4>{varying_nrms_}) };
		matrix<4, 4> tan_space_basis{
			normalize(tan_bitan[0]),
			normalize(tan_bitan[1]),
			weighted_normal,
			{ 0, 0, 0, 1 } // Darboux frame ??
		};

		// NORMAL MAP TEXTURE
		vec4 normal_tan_basis{ normalize(model_.normal_tex(uv_coords) * tan_space_basis) };
		// DIFF MAP
		TGAColor diffMap{ model_.diff(uv_coords) };
		// SPEC SHADER
		double specIntensity{ model_.spec(uv_coords) };

		// LIGHTING
		// AMBIENT
		constexpr double ambient{ 0.4 };

		// DIFFUSE
		double diffuse{ (std::max(0., dot(normal_tan_basis, world_light_))) };

		// SPECULAR
		constexpr int shininess{ 35 };
		const vec4 reflected_ray{
			normalize(
			2. * dot(normal_tan_basis, world_light_) * normal_tan_basis - world_light_
			)
		};
		double specular{
			model_.spec(uv_coords)
			// obj points to simply +z axis (in eye coords) since camera @ (0, 0). r.z since (0,0,1) (viewer dir)*(r.x, r.y, r.z) = r.z
			* std::pow(std::max(0., reflected_ray.z),
			shininess
		) };

		TGAColor final_FragColor{ diffMap };
		for (int i : {0, 1, 2}) {
			final_FragColor[i] *= std::min(1., (ambient + 1 * diffuse + 3 * specular));
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
