#pragma once
#include <cmath>
#include <cassert>
#include <iostream>
#include <array>
#include <vector>

template<int n> struct vec {
	double data[n]{};
	double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
	double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<int n> vec<n>& operator+=(vec<n>& lhs, const vec<n>& rhs) {
	for (int i{ 0 }; i < n; ++i) {
		lhs[i] += rhs[i];
	}
	return lhs;
}
template<int n> vec<n>& operator-=(vec<n>& lhs, const vec<n>& rhs) {
	for (int i{ 0 }; i < n; ++i) {
		lhs[i] -= rhs[i];
	}
	return lhs;
}
template<int n, typename T> vec<n>& operator*=(vec<n>& lhs, T scalar) {
	for (int i{ 0 }; i < n; ++i) {
		lhs[i] *= scalar;
	}
	return lhs;
}

template<int n> vec<n> operator+(vec<n> lhs, const vec<n>& rhs) {
	lhs += rhs;
	return lhs;
}

template<int n> vec<n> operator-(vec<n> lhs, const vec<n>& rhs) {
	lhs -= rhs;
	return lhs;
}

// scalar multiplication
template<int n, typename T> vec<n> operator*(T scalar, vec<n> vect) {
	vect *= scalar;
	return vect;
}
template<int n, typename T> vec<n> operator*(vec<n> vect, T scalar) {
	vect *= scalar;
	return vect;
}

// dot product
template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
	double res{};
	for (int i{ 0 }; i < n; ++i) {
		res += lhs[i] * rhs[i];
	}
	return res;
}

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
	for (std::size_t i = 0; i < n; ++i) out << v[i] << " ";
	return out;
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
		struct { double x, y, z, w; };
		double data[4];

	};
	double& operator[](const int i) { assert(i >= 0 && i < 4); return data[i]; }
	double operator[](const int i) const { assert(i >= 0 && i < 4); return data[i]; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;



// MATRICES
template<int R, int C> struct matrix {
	std::array<vec<C>, R> data{ }; // R rows of vec<C>
	int nrows() { return R; }
	int ncols() { return C; }
	vec<C>& operator[](const int i) { assert(i >= 0 && i < R); return data[i]; }
	vec<C> operator[](const int i) const { assert(i >= 0 && i < R); return data[i]; }

	matrix<C, R> transpose() {
		matrix<C, R> res{};
		for (int i{ 0 }; i < R; ++i) {
			for (int j{ 0 }; j < C; ++j) {
				res[j][i] = (*this)[i][j];
			}
		}
		return res;
	}

	// targetRow + factor * fromRow
	void addRows(vec<C>& target, double factor, const vec<C>& from) {
		for (int c{ 0 }; c < C; ++c) {
			target[c] += factor * from[c];
		}
	}

	double determinant() {
		assert(R == C);

		matrix<R, C> matrix{ *this }; // make copy to not override
		double determ{ 1 };
		int switches{};

		for (int pivot{ 0 }; pivot < R; ++pivot) {
			double maxV{ std::abs(matrix[pivot][pivot]) };

			// make matrix[pivot] row hold largest abs val at col pivot
			for (int rowIdx{ pivot + 1 }; rowIdx < R; ++rowIdx) {
				if (std::abs(matrix[rowIdx][pivot]) > maxV) {
					maxV = std::abs(matrix[rowIdx][pivot]);
					std::swap(matrix[rowIdx], matrix[pivot]);
					++switches;
				}
			}

			// for every row below pivot, make val at pivot col 0
			for (int rowIdx{ pivot + 1 }; rowIdx < R; ++rowIdx) {
				double factor = -(matrix[rowIdx][pivot]) / matrix[pivot][pivot];
				addRows(matrix[rowIdx], factor, matrix[pivot]);
			}
		}

		// now in row echelon form
		for (int i{ 0 }; i < nrows(); ++i) {
			determ *= matrix[i][i];
		}

		return determ * ((switches % 2) == 0 ? 1. : -1.);
	}

	// helper to get minor matrix of the big "this" matrix
	matrix<R - 1, C - 1> minorMatrix(const int ai, const int aj) {
		matrix<R - 1, C - 1> minorMatrix{};

		for (int i{ 0 }, curRowIdx{ 0 }; i < R - 1; ++i, ++curRowIdx) {
			if (curRowIdx == ai) { ++curRowIdx; }
			for (int j{ 0 }; j < C - 1; ++j) {
				if (j < aj) {
					minorMatrix[i][j] = (*this)[curRowIdx][j];
				}
				else {
					minorMatrix[i][j] = (*this)[curRowIdx][j + 1];
				}
			}
		}

		return minorMatrix;
	}

	matrix<C, R> adjoint() {
		assert(C == R && "Adjoint only exists for sqr matrix");
		matrix<C, R> adj{};

		for (int i{ 0 }; i < R; ++i) {
			for (int j{ 0 }; j < C; ++j) {
				matrix<R - 1, C - 1> minorMatrix{};
				double minorVal{ (*this).minorMatrix(i, j).determinant() };
				int cofactor{ (i + j) % 2 == 0 ? 1 : -1 };

				adj[j][i] = minorVal * cofactor;
			}
		}

		return adj;
	}

	matrix<R, C> inverse() {
		double determ{ (*this).determinant() };
		assert(determ != 0 && "Determinant of matrix isn't 0; no inverse exists");

		matrix<C, R> adj{ (*this).adjoint() };
		std::cout << '\n' << determ << '\n';
		return (1 / determ) * adj;
	}

	// SCALAR MULTIPLICATION
	template <typename T>
	matrix<R, C>& operator*=(T scalar) {
		for (vec<C>& row : (*this).data) {
			row *= scalar;
		}

		return *this;
	}

	friend std::ostream& operator<<(std::ostream& out, const matrix<R, C>& m) {
		for (std::size_t i = 0; i < R; ++i) out << m[i] << "\n";
		return out;
	}

};

template<int r1, int K, int c2>
matrix<r1, c2> operator*(const matrix<r1, K>& m1, const matrix<K, c2>& m2) {
	matrix<r1, c2> res{};

	for (int i{ 0 }; i < r1; ++i) {
		for (int j{ 0 }; j < c2; ++j) {
			for (int k{ 0 }; k < K; ++k) {
				res[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return res;
}

template <int R, int C, typename T>
matrix<R, C> operator*(matrix<R, C> m, T scalar) {
	m *= scalar;

	return m;
}
template <int R, int C, typename T>
matrix<R, C> operator*(T scalar, matrix<R, C> m) {
	m *= scalar;

	return m;
}