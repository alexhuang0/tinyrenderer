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

template<int n> vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs) {
	vec<n> res{};
	for (int i{ 0 }; i < n; ++i) {
		res[i] = lhs[i] + rhs[i];
	}
	return res;
}

template<int n> vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs) {
	vec<n> res{};
	for (int i{ 0 }; i < n; ++i) {
		res[i] = lhs[i] - rhs[i];
	}
	return res;
}

// scalar multiplication
template<int n, typename T> vec<n> operator*(T scalar, const vec<n>& vect) {
	vec<n> res{};
	for (int i{ 0 }; i < n; ++i) {
		res[i] = scalar * vect[i];
	}
	return res;
}
template<int n, typename T> vec<n> operator*(const vec<n>& vect, T scalar) {
	vec<n> res{};
	for (int i{ 0 }; i < n; ++i) {
		res[i] = scalar * vect[i];
	}
	return res;
}

// dot product
template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
	double res{};
	for (int i{ 0 }; i < n; ++i) {
		res += lhs[i] * rhs[i];
	}
	return res;
}

template<> struct vec<2> {
	union {
		struct { double x, y; };
		double data[2];

	};
	double& operator[](const int i) { assert(i >= 0 && i < 2); return data[i]; }
	double operator[](const int i) const { assert(i >= 0 && i < 2); return data[i]; }
};

template<> struct vec<3> {
	union {
		struct { double x, y, z; };
		double data[3];

	};
	double& operator[](const int i) { assert(i >= 0 && i < 3); return data[i]; }
	double operator[](const int i) const { assert(i >= 0 && i < 3); return data[i]; }
};

template<> struct vec<4> {
	union {
		struct { double x, y, z, w };
		double data[4];

	};
	double& operator[](const int i) { assert(i >= 0 && i < 4); return data[i]; }
	double operator[](const int i) const { assert(i >= 0 && i < 4); return data[i]; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;


