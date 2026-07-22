#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec {
	double data[n]{};
	double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
	double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
	for (std::size_t i = 0; i < n; ++i) out << v[i] << " ";
	return out;
}

template<> struct vec<3> {
	union {
		struct { double x, y, z; };
		double data[3];

	};
	double& operator[](const int i) { assert(i >= 0 && i < 3); return data[i]; }
	double operator[](const int i) const { assert(i >= 0 && i < 3); return data[i]; }
};

typedef vec<3> vec3;