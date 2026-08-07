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
	// frag pos in eye space
	vec4 coords{ tri_[0] * bary_coords.x + tri_[1] * bary_coords.y + tri_[2] * bary_coords.z };

	// convert from eye space -> object/model space -> light space -> light Clip space (perspective, but not inside [-1, 1] cube)
	vec4 coords_light_pov{ shContext_.Perspective * shContext_.ModelView * rContext_.ModelView.inverse() * coords };

	// convert to [-1, 1] cube
	vec4 ndc_light{ coords_light_pov / coords_light_pov.w };

	// scale to fit screen
	vec4 screen{ shContext_.Viewport * ndc_light };

	int shX{ static_cast<int>(screen.x) }, shY{ static_cast<int>(screen.y) };
	double shZ{ screen.z };

	bool in_shadow{ false };
	double shadow_bias{ 0.03 };

	// if inside shadow map bounds
	if (shX >= 0 && shX < Canvas::width && shY >= 0 && shY < Canvas::height) {
		if (shContext_.zbuffer[shY * Canvas::width + shX] > shZ + shadow_bias) {
			in_shadow = true;
		}
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
	double light_intensity{ ambient };

	if (!in_shadow) {
		light_intensity += 1 * diffuse + 3 * specular;
	}
	for (int i : {0, 1, 2}) {
		final_FragColor[i] *= std::min(1., light_intensity);
	}

	return { false, final_FragColor };
}



vec4 ShadowShader::vertex(int iface, int ivert) {
	vec4 v = model_.vert(iface, ivert); // cur vertex in obj coord
	vec4 global_pos = shContext_.ModelView * v; // light's POV

	return shContext_.Perspective * global_pos;
}

std::pair<bool, TGAColor> ShadowShader::fragment(const vec3& bary_coords) const {
	TGAColor final_FragColor{};

	return { false, final_FragColor };
}