#ifndef FCP_MATH_GEOMETRY_REPR_CASTS_HPP
#define FCP_MATH_GEOMETRY_REPR_CASTS_HPP

#include "core/common.hpp"
#include "core/forward.hpp"
#include "core/math_fun.hpp"

#include "linalg/matrix.hpp"

#include <array>
#include <numbers>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Radians to Degrees
//----------------------------------------------------------------------------------

// From: https://developer.download.nvidia.com/cg/degrees.html
// Convert radians to degrees
template <typename T>
constexpr T rad_to_deg(T x)
{
	return static_cast<T>(57.29577951L) * x;
}

//----------------------------------------------------------------------------------
// Degrees to Radians
//----------------------------------------------------------------------------------

// From: https://developer.download.nvidia.com/cg/radians.html
// Convert degrees to radians
template <typename T>
constexpr T deg_to_rad(T x)
{
	return static_cast<T>(0.017453292L) * x;
}

//----------------------------------------------------------------------------------
// Euler Angles to Matrix
//----------------------------------------------------------------------------------

// Create a matrix from a given set of Euler angles
// CONVENTIONS:
// - Euler angles order: heading-pitch-bank (roll-pitch-yaw)
// - Y-Axis is UP
// - Matrix rotation: object space to upright space
// - Returns a column-major matrix by default
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	typename T, 
	LazyMatrixType M = 
#if FCPM_GRAPHICS_USE_ROW_MAJOR	
	RowMatrix<T, 4, 4>
#else
	ColMatrix<T, 4, 4>
#endif
>
constexpr LazyMatrixType auto matrix_from_euler(T heading, T pitch, T bank)
{
	const auto cos_h{ fcp::math::cos(heading) };
	const auto cos_p{ fcp::math::cos(pitch) };
	const auto cos_b{ fcp::math::cos(bank) };

	const auto sin_h{ fcp::math::sin(heading) };
	const auto sin_p{ fcp::math::sin(pitch) };
	const auto sin_b{ fcp::math::sin(bank) };

	constexpr T zero{ static_cast<T>(0) };
	constexpr T one{ static_cast<T>(1) };

	if constexpr (ColumnVectorFormat)
	{
		return M{
			cos_h*cos_b + sin_h*sin_p*sin_b, sin_h*sin_p*cos_b - cos_h*sin_b, sin_h*cos_p, zero,
			sin_b*cos_p, 										 cos_b*cos_p, 										-sin_p, 		 zero,
			cos_h*sin_p*sin_b - sin_h*cos_b, sin_b*sin_h + cos_h*sin_p*cos_b, cos_h*cos_p, zero,
			zero, 													 zero, 														zero, 			 one
		};
	} else {
		return M{
			cos_h*cos_b + sin_h*sin_p*sin_b, sin_b*cos_p, cos_h*sin_p*sin_b - sin_h*cos_b, zero,
			sin_h*sin_p*cos_b - cos_h*sin_b, cos_b*cos_p, sin_b*sin_h + cos_h*sin_p*cos_b, zero,
			sin_h*cos_p, 										 -sin_p, 			cos_h*cos_p, 									 	 zero,
			zero, 													 zero,        zero,                            one
		};
	}
}

// Create a matrix from a given set of Euler angles
// CONVENTIONS:
// - Euler angles order: heading-pitch-bank (roll-pitch-yaw)
// - Y-Axis is UP
// - Matrix rotation: object space to upright space
// - Returns a column-major matrix by default, can be overridden
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	typename T, 
	LazyMatrixType M = ColMatrix<T, 4, 4>
>
constexpr LazyMatrixType auto matrix_from_euler(const std::array<T, 3>& angles)
{
	return matrix_from_euler<ColumnVectorFormat, T, M>(angles[0], angles[1], angles[2]);
}

//----------------------------------------------------------------------------------
// Matrix to Euler Angles
//----------------------------------------------------------------------------------

// Create an array of Euler angles from a given matrix
// CONVENTIONS:
// - Euler angles order: heading-pitch-bank (roll-pitch-yaw)
// - Y-Axis is UP
// - Matrix rotation: object space to upright space
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	LazyMatrixType M
>
constexpr auto euler_from_matrix(const M& matrix)
{
	using T = internal::Traits<std::remove_cvref_t<M>>::element_type;
	enum { H, P, B } index{ H };
	std::array<T, 3> result;
	constexpr T pio2{ std::numbers::pi_v<T> / static_cast<T>(2) };

	// 1. Pitch
	T temp;
	if constexpr (ColumnVectorFormat)
	{
		temp = -matrix[1, 2];	
	} else {
		temp = -matrix[2, 1];
	}

	// Clamp to avoid NaN results
	if (less_or_equal(temp, static_cast<T>(-1)))
		result[P] = static_cast<T>(-pio2);
	else if (greater_or_equal(temp, static_cast<T>(1)))
		result[P] = static_cast<T>(pio2);
	else
		result[P] = fcp::math::asin(temp);

	// 2. Heading and Bank with Gymbal Lock check
	if constexpr (ColumnVectorFormat)
	{
		if (fcp::math::abs(temp) > static_cast<T>(0.9999))
		{
			// Gymbal Lock (object "looking" straight up or down)
			result[H] = fcp::math::atan2(-matrix[2, 0], matrix[0, 0]);	
			result[B] = static_cast<T>(0);	// Set to default value
		} else {
			result[H] = fcp::math::atan2(matrix[0, 2], matrix[2, 2]);
			result[B] = fcp::math::atan2(matrix[1, 0], matrix[1, 1]);
		}
	} else {
		if (fcp::math::abs(temp) > static_cast<T>(0.9999))
		{
			// Gymbal Lock (object "looking" straight up or down)
			result[H] = fcp::math::atan2(-matrix[0, 2], matrix[0, 0]);	
			result[B] = static_cast<T>(0);	// Set to default value
		} else {
			result[H] = fcp::math::atan2(matrix[2, 0], matrix[2, 2]);
			result[B] = fcp::math::atan2(matrix[0, 1], matrix[1, 1]);
		}
	}

	return result;
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_GEOMETRY_REPR_CASTS_HPP
