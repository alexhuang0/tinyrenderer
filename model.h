#pragma once
#include "geometry/geometry.h"
#include <vector>
#include <tuple>

class Model {
	std::vector<vec3> verts{}; // array of vertices
	std::vector<vec3> norms{};
	std::vector<std::size_t> facet_vrt{}; // per-triangle idx in above arrays
	std::vector<std::size_t> facet_nrm{}; // per-triangle normal idx in norms

public:
	Model(const std::string& filename);
	constexpr std::size_t nverts() const { return verts.size(); } // num of vertices
	constexpr std::size_t nfaces() const { return facet_vrt.size() / 3; } // num of faces
	vec3 vert(const std::size_t i) const; // 0 <= i < nverts()
	vec3 vert(const std::size_t iface, const std::size_t nthvert) const; // 0 <= iface < nfaces(), 0 <= nthvert < 3
	vec3 normal(const std::size_t iface, const std::size_t nthnorm) const;
};