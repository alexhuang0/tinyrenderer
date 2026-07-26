#pragma once

#include <array>
#include <cmath>
#include <cassert>
#include <iostream>

#include "vector.h"

using Idx = std::size_t;
using Size = std::size_t;

template<Size R, Size C> struct matrix {
	std::array<vec<C>, R> data{ }; // R rows of vec<C>
	constexpr Size nrows() const { return R; }
	constexpr Size ncols() const { return C; }
	vec<C>& operator[](Idx i) { assert(i >= 0 && i < R); return data[i]; }
	const vec<C>& operator[](Idx i) const { assert(i >= 0 && i < R); return data[i]; }

	matrix<C, R> transpose() {
		matrix<C, R> res{};
		for (Idx i{ 0 }; i < R; ++i) {
			for (Idx j{ 0 }; j < C; ++j) {
				res[j][i] = data[i][j];
			}
		}
		return res;
	}

	// targetRow + factor * fromRow
	static void addRows(vec<C>& target, double factor, const vec<C>& from) {
		for (Idx c{ 0 }; c < C; ++c) {
			target[c] += factor * from[c];
		}
	}

	double determinant() const {
		assert(R == C);

		matrix<R, C> matrix{ data }; // make copy to not override
		double determ{ 1 };
		int switches{};

		for (Idx pivot{ 0 }; pivot < R; ++pivot) {
			double maxV{ std::abs(matrix[pivot][pivot]) };

			// make matrix[pivot] row hold largest abs val at col pivot
			for (Idx rowIdx{ pivot + 1 }; rowIdx < R; ++rowIdx) {
				if (std::abs(matrix[rowIdx][pivot]) > maxV) {
					maxV = std::abs(matrix[rowIdx][pivot]);
					std::swap(matrix[rowIdx], matrix[pivot]);
					++switches;
				}
			}

			// for every row below pivot, make val at pivot col 0
			for (Idx rowIdx{ pivot + 1 }; rowIdx < R; ++rowIdx) {
				double factor = -(matrix[rowIdx][pivot]) / matrix[pivot][pivot];
				addRows(matrix[rowIdx], factor, matrix[pivot]);
			}
		}

		// now in row echelon form
		for (Idx i{ 0 }; i < nrows(); ++i) {
			determ *= matrix[i][i];
		}

		return determ * ((switches % 2) == 0 ? 1. : -1.);
	}

	// helper to get minor matrix of the big "this" matrix
	matrix<R - 1, C - 1> minorMatrix(int ai, int aj) const {
		matrix<R - 1, C - 1> minorMatrix{};

		for (Idx i{ 0 }, curRowIdx{ 0 }; i < R - 1; ++i, ++curRowIdx) {
			if (curRowIdx == ai) { ++curRowIdx; }
			for (Idx j{ 0 }; j < C - 1; ++j) {
				if (j < aj) {
					minorMatrix[i][j] = data[curRowIdx][j];
				}
				else {
					minorMatrix[i][j] = data[curRowIdx][j + 1];
				}
			}
		}

		return minorMatrix;
	}

	matrix<C, R> adjoint() const {
		assert(C == R && "Adjoint only exists for sqr matrix");
		matrix<C, R> adj{};

		for (Idx i{ 0 }; i < R; ++i) {
			for (Idx j{ 0 }; j < C; ++j) {
				double minorVal{ minorMatrix(i, j).determinant() };
				int cofactor{ (i + j) % 2 == 0 ? 1 : -1 };

				adj[j][i] = minorVal * cofactor;
			}
		}

		return adj;
	}

	matrix<R, C> inverse() const {
		double determ{ determinant() };
		assert(determ != 0 && "Determinant of matrix isn't 0; no inverse exists");

		matrix<C, R> adj{ adjoint() };
		return (1 / determ) * adj;
	}

	// SCALAR MULTIPLICATION
	template <typename T>
	matrix<R, C>& operator*=(T scalar) {
		for (vec<C>& row : data) {
			row *= scalar;
		}

		return *this;
	}

	friend std::ostream& operator<<(std::ostream& out, const matrix<R, C>& m) {
		for (Idx i = 0; i < R; ++i) out << m[i] << "\n";
		return out;
	}

};

template<Size r1, Size K, Size c2>
matrix<r1, c2> operator*(const matrix<r1, K>& m1, const matrix<K, c2>& m2) {
	matrix<r1, c2> res{};

	for (Idx i{ 0 }; i < r1; ++i) {
		for (Idx j{ 0 }; j < c2; ++j) {
			for (Idx k{ 0 }; k < K; ++k) {
				res[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return res;
}

template <Size R, Size C, typename T>
matrix<R, C> operator*(matrix<R, C> m, T scalar) {
	m *= scalar;

	return m;
}
template <Size R, Size C, typename T>
matrix<R, C> operator*(T scalar, matrix<R, C> m) {
	m *= scalar;

	return m;
}