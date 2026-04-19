#ifndef FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_SIMPLE_HPP
#define FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_SIMPLE_HPP

#include "core/internal/common.hpp"

#include "linalg/matrix.hpp"

#include <array>
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Scale
//----------------------------------------------------------------------------------

// Scale by factor k (scalar) along generic direction n
//NOTE: returns a column-major matrix by default
template <typename T, Lazy3DVectorLike Vector, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto scale(const Matrix& matrix, T k, Vector n)
{
	enum { X, Y, Z };
	auto temp{ k - 1 };
	constexpr T zero{ static_cast<T>(0) };

	// Here it's best to just return a GemmExpr object
	return matrix * Matrix{
		1 + temp*n[X]*n[Y], temp*n[Y]*n[X], temp*n[Z]*n[X], zero,
		temp*n[X]*n[Y], 1 + temp*n[Y]*n[Y], temp*n[Z]*n[Y], zero,
		temp*n[X]*n[Z], temp*n[Y]*n[Z], 1 + temp*n[Z]*n[Z], zero,
		zero, zero, zero, static_cast<T>(1)
	};
}

// Scale by factor k (vector) along cardinal axes
//NOTE: returns a column-major matrix by default
template <typename T, Lazy3DVectorLike Vector, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto scale(const Matrix& matrix, Vector k)
{
	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix::identity() };

	// Compute the result optimizing for the exact layout 
	//NOTE: column-major is the best layout here
	if constexpr (mtraits::is_row_major)
	{
		for (int i{0}; i < 4; i++)
			for (int j{0}; j < 3; j++)
				result[i, j] = matrix[i, j] * k[j];

	} else {
		for (int i{0}; i < 3; i++)
			for (int j{0}; j < 4; j++)
				result[j, i] = matrix[j, i] * k[i];
	}

		// Common part (fill last column)
		for (int j{0}; j < 4; j++)
			result[j, 3] = matrix[j, 3];

	return result;
}

//----------------------------------------------------------------------------------
// Shear / Skew
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
//CONVENTION: Axis = { X = 0, Y = 1, Z = 2 }
template <int Axis, typename T, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto shear_impl(const Matrix& matrix, T s, T t)
{
	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix::identity() };
	constexpr auto zero{ static_cast<T>(0) };

	constexpr enum { X, Y, Z } axis{ Axis };

	std::array<T, 4> factors;
	
	if constexpr (axis == X) factors = { zero, zero, s, t }; 
	if constexpr (axis == Y) factors = { s, zero, t, zero }; 
	if constexpr (axis == Z) factors = { s, t, zero, zero };

	// Compute the result optimizing for the exact layout 
	//NOTE: column-major is the best layout here
	if constexpr (mtraits::is_row_major)
	{
		for (int i{0}; i < 4; i++)	// Rows
			for (int j{0}; j < 4; j++)	// Columns
				result[i, j] = matrix[i, j] + factors[j]*matrix[i, axis];
	} else {
		for (int i{0}; i < 4; i++)	// Columns
			for (int j{0}; j < 4; j++)	// Rows
				result[j, i] = matrix[j, i] + factors[i]*matrix[j, axis];
	}	

	return result;
}
END_FCP_INTERNAL_NAMESPACE

// Shear x and y coordinates by z coordinate
//NOTE: returns a column-major matrix by default
template <typename T, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto shear_xy_by_z(const Matrix& matrix, T s, T t)
{
	return internal::shear_impl<2, T, Matrix>(matrix, s, t);
}

// Shear x and z coordinates by y coordinate
//NOTE: returns a column-major matrix by default
template <typename T, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto shear_xz_by_y(const Matrix& matrix, T s, T t)
{
	return internal::shear_impl<1, T, Matrix>(matrix, s, t);
}

// Shear y and z coordinates by x coordinate
//NOTE: returns a column-major matrix by default
template <typename T, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto shear_yz_by_x(const Matrix& matrix, T s, T t)
{
	return internal::shear_impl<0, T, Matrix>(matrix, s, t);
}

//----------------------------------------------------------------------------------
// Reflection
//----------------------------------------------------------------------------------

// Reflect about a generic plane parallel to normalized vector n
//NOTE: returns a column-major matrix by default
template <typename T, Lazy3DVectorLike Vector, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto reflect(const Matrix& matrix, const Vector& n)
{
	enum { X, Y, Z };
	constexpr auto zero{ static_cast<T>(0) };
	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	// Here it's best to just return a GemmExpr object
	return matrix * Matrix{
		one-two*n[X]*n[X], -two*n[X]*n[Y], -two*n[X]*n[Z], zero,
		-two*n[Y]*n[X], one-two*n[Y]*n[Y], -two*n[Y]*n[Z], zero,
		-two*n[Z]*n[X], -two*n[Z]*n[Y], one-two*n[Z]*n[Z], zero,
		zero, zero, zero, one
	};	
}

//----------------------------------------------------------------------------------
// Translation
//----------------------------------------------------------------------------------

// Translate along generic direction n
//NOTE: returns a matrix which is adapted to the type of vector of n
//NOTE: returns a column-major matrix by default
template <typename T, Lazy3DVectorLike Vector, Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>>
constexpr auto translate(const Matrix& matrix, const Vector& n)
{
	using vtraits = internal::Traits<std::remove_cvref_t<Vector>>;
	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	enum { X, Y, Z };
	auto result{ matrix };
	std::array<T, 4> factors = { n[X], n[Y], n[Z], static_cast<T>(0) };

	if constexpr (vtraits::is_row_major)	// Row vector
	{
		if constexpr (mtraits::is_row_major)
		{
			for (int i{0}; i < 4; i++)	// Rows	
				for (int j{0}; j < 3; j++)	// Columns
					result[i, j] += factors[j]*result[i, 3];
		} else {
			for (int i{0}; i < 3; i++)	// Columns
				for (int j{0}; j < 4; j++)	// Rows
					result[j, i] += factors[i]*result[j, 3];
		}	
	} else {															// Column vector
		for (int i{0}; i < 4; i++)
			result[i, 3] = result[i, 0]*n[X] + result[i, 1]*n[Y] + 
										 result[i, 2]*n[Z] + result[i, 3];
	}

	return result;
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_SIMPLE_HPP
