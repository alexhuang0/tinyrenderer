#include "shaders.h"

vec4 PhongShader::vertex(int iface, int ivert) {
	vec4 v = model_.vert(iface, ivert); // cur vertex in obj coord
	vec4 global_pos = rContext_.ModelView * v;

	tri_[ivert] = global_pos;

	varying_nrms_[ivert] = rContext_.ModelView.inverse().transpose() * model_.normal(iface, ivert);

	vec2 uvCoord{ model_.uv_coords(iface, ivert) };
	varying_uv_[ivert] = uvCoord;

	return rContext_.Perspective * global_pos;
}

std::pair<bool, TGAColor> PhongShader::fragment(const vec3& bary_coords) const {
	matrix<3, 4> tri{ tri_ };
	vec3 coords{ tri * vec4{bary_coords.x, bary_coords.y, bary_coords.z, 1. } };
	if (shContext_.zbuffer[coords.y * Canvas::width + coords.x] > coords.z) {
		return { true, {0, 0, 0, 0} };
	}

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



vec4 ShadowShader::vertex(int iface, int ivert) {
	vec4 v = model_.vert(iface, ivert); // cur vertex in obj coord
	vec4 global_pos = rContext_.ModelView * rContext_.ModelView.inverse() * v; // light's POV

	tri_[ivert] = global_pos;

	return global_pos;
}

std::pair<bool, TGAColor> ShadowShader::fragment(const vec3& bary_coords) const {
	TGAColor final_FragColor{};

	return { false, final_FragColor };
}