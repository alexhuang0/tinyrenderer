#pragma once
#include "geometry/geometry.h"
#include <vector>
#include "tgaimage.h"

class Model {
	std::vector<vec3> verts{}; // array of vertices
	std::vector<vec3> norms{}; // array of normal vectors
	std::vector<vec2> tex{}; // array of tex coords on the uv tga map
	std::vector<std::size_t> facet_vrt{}; // per-triangle idx in verts
	std::vector<std::size_t> facet_nrm{}; // per-triangle normal idx in norms (taken from vn x y z)
	std::vector<std::size_t> facet_tex{}; // per-triangle tex idx in tex (uv tga map)
	TGAImage normalmap{};
	TGAImage diffmap{}; // differential shader (color)

public:
	Model(const std::string& filename);
	constexpr std::size_t nverts() const { return verts.size(); } // num of vertices
	constexpr std::size_t nnorms() const { return norms.size(); } // num of vertices
	constexpr std::size_t ntex() const { return tex.size(); } // num of vertices
	constexpr std::size_t nfaces() const { return facet_vrt.size() / 3; } // num of faces
	vec3 vert(const std::size_t i) const; // 0 <= i < nverts()
	vec3 vert(const std::size_t iface, const std::size_t nthvert) const; // 0 <= iface < nfaces(), 0 <= nthvert < 3
	vec3 normal(const std::size_t iface, const std::size_t nthnorm) const; // normal coming from "vn x y z"
	vec4 normal_tex(const vec2& uv) const; // normal vector from normal map texture (normalized [-1..1])
	vec2 uv_coords(const std::size_t iface, const std::size_t nthvert) const; // uv coords on a TGA normalmap for a given triangle's vertex
};