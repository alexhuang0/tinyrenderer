#pragma once
#include "geometry.h"
#include <vector>
#include <tuple>

class Model {
	std::vector<vec3> verts{}; // array of vertices
	std::vector<std::size_t> facet_vrt{}; // per-triangle idx in above array

public:
	Model(const std::string& filename);
	std::size_t nverts() const { return verts.size(); } // num of vertices
	std::size_t nfaces() const { return facet_vrt.size() / 3; } // num of faces
	vec3 vert(const std::size_t i) const; // 0 <= i < nverts()
	vec3 vert(const std::size_t iface, const std::size_t nthvert) const; // 0 <= iface < nfaces(), 0 <= nthvert < 3
};