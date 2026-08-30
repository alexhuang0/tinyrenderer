#pragma once
#include "our_gl.h"
#include "model.h"

struct PhongShader : IShader {
	const Model& model_;
	const RenderContext& rContext_;
	const RenderContext& shContext_;

	vec4 world_light_; // sun in eye space
	std::array<vec4, 3> tri_{}; // triangle in eye space
	std::array<vec4, 3> varying_nrms_{}; // tri vertices' normals in eye space
	std::array<vec2, 3> varying_uv_{}; // tri uv coordinates, written by vertex shader, read by fragment shader

	PhongShader(const Model& m, const RenderContext& rc, const RenderContext& sh, const vec4& sun)
		: model_{ m }
		, rContext_{ rc }
		, shContext_{ sh }
	{
		// w = 0. bc its a vector
		vec4 light_4d{ rContext_.ModelView * sun };
		world_light_ = normalize(light_4d);
	}

	vec4 vertex(int iface, int ivert);

	std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const override;
};

struct ShadowShader : IShader {
	const Model& model_;
	const RenderContext& shContext_;

	ShadowShader(const Model& m, const RenderContext& sh)
		: model_{ m }
		, shContext_{ sh }
	{
	}

	vec4 vertex(int iface, int ivert);

	std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const override;
};

struct SSAOShader : IShader {
	const Model& model_;
	const RenderContext& rContext_;

	std::array<vec4, 3> tri_{}; // triangle in eye space
	std::array<vec4, 3> varying_nrms_{}; // tri vertices' normals in eye space
	std::array<vec2, 3> varying_uv_{}; // tri uv coordinates, written by vertex shader, read by fragment shader

	SSAOShader(const Model& m, const RenderContext& rc)
		: model_{ m }
		, rContext_{ rc }
	{
	}

	vec4 vertex(int iface, int ivert);

	std::pair<bool, TGAColor> fragment(const vec3& bary_coords) const override;
};