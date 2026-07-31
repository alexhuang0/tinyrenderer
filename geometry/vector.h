#pragma once
#include <cmath>
#include <cassert>
#include <iostream>
#include <array>

using Idx = std::size_t;
using Size = std::size_t;

template<Size n> struct vec {
	double data[n]{};
	double& operator[](Idx i) { assert(i >= 0 && i < n); return data[i]; }
	double  operator[](Idx i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<> struct vec<2> {
	union {
		struct { double x, y; };
		double data[2];

	};
	double& operator[](Idx i) { assert(i >= 0 && i < 2); return data[i]; }
	double operator[](Idx i) const { assert(i >= 0 && i < 2); return data[i]; }
};

template<> struct vec<3> {
	union {
		struct { double x, y, z; };
		double data[3];

	};
	double& operator[](Idx i) { assert(i >= 0 && i < 3); return data[i]; }
	double operator[](Idx i) const { assert(i >= 0 && i < 3); return data[i]; }
};

template<> struct vec<4> {
	union {
		struct { double x, y, z, w; };
		double data[4];

	};
	double& operator[](Idx i) { assert(i >= 0 && i < 4); return data[i]; }
	double operator[](Idx i) const { assert(i >= 0 && i < 4); return data[i]; }
};

using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

template<Size n> vec<n>& operator+=(vec<n>& lhs, const vec<n>& rhs) {
	for (Idx i{ 0 }; i < n; ++i) {
		lhs[i] += rhs[i];
	}
	return lhs;
}
template<Size n> vec<n>& operator-=(vec<n>& lhs, const vec<n>& rhs) {
	for (Idx i{ 0 }; i < n; ++i) {
		lhs[i] -= rhs[i];
	}
	return lhs;
}
template<Size n, typename T> vec<n>& operator*=(vec<n>& lhs, T scalar) {
	for (Idx i{ 0 }; i < n; ++i) {
		lhs[i] *= scalar;
	}
	return lhs;
}

template<Size n> vec<n> operator+(vec<n> lhs, const vec<n>& rhs) {
	lhs += rhs;
	return lhs;
}

template<Size n> vec<n> operator-(vec<n> lhs, const vec<n>& rhs) {
	lhs -= rhs;
	return lhs;
}

// scalar multiplication
template<Size n, typename T> vec<n> operator*(T scalar, vec<n> vect) {
	vect *= scalar;
	return vect;
}
template<Size n, typename T> vec<n> operator*(vec<n> vect, T scalar) {
	vect *= scalar;
	return vect;
}

// dot product
template<Size n> double dot(const vec<n>& lhs, const vec<n>& rhs) {
	double res{};
	for (Idx i{ 0 }; i < n; ++i) {
		res += lhs[i] * rhs[i];
	}
	return res;
}

template<Size n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
	for (Idx i = 0; i < n; ++i) out << v[i] << " ";
	return out;
}

vec3 rot(vec3 v);
