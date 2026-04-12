#ifndef FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_SPACE_CONVERSIONS_HPP
#define FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_SPACE_CONVERSIONS_HPP

#include "core/common.hpp"
#include "core/hardware.hpp"

#include "linalg/matrix.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

enum class DepthInterval { NegOneToOne, ZeroToOne };

#if FCPM_GRAPHICS_USE_NOTO_DEPTH_INTERVAL
#define FCPMG_DEFAULT_DEPTH_INT DepthInterval::NegOneToOne
#else
#define FCPMG_DEFAULT_DEPTH_INT DepthInterval::ZeroToOne
#endif

//----------------------------------------------------------------------------------
// Orthographic Projection
//----------------------------------------------------------------------------------

// Standard orthographic projection
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT,
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto ortho(T left, T right, T bottom, T top, T near, T far)
{
	//TODO:error handling with assert and [[unlikely]]

	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix::identity() };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	const T inv_rl{ one / (right - left) };
	const T inv_tb{ one / (top - bottom) };
	const T inv_fn{ one / (far - near) };

	// 1. Common diagonal elements (scaling components)
	result[0, 0] = two * inv_rl;	// X scaling
	result[1, 1] = two * inv_tb;	// Y scaling

	// Z scaling (depends both on target range and handedness)
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = two * inv_fn;
		} else {
			result[2, 2] = static_cast<T>(-2) * inv_fn;
		}
	} else {																			 // Range: [0, 1]
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = one * inv_fn;
		} else {
			result[2, 2] = static_cast<T>(-1) * inv_fn;
		}
	}

	// 2. Non-diagonal elements (translation components)
	const auto tx{ -(right + left) * inv_rl }; // X translation 
	const auto ty{ -(top + bottom) * inv_tb }; // Y translation 
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = -(far + near) * inv_fn;
	} else {																			 // Range: [0, 1]
			tz = (-near) * inv_fn;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[0, 3] = tx;
		result[1, 3] = ty;
		result[2, 3] = tz;
	} else {
		result[3, 0] = tx;
		result[3, 1] = ty;
		result[3, 2] = tz;
	}

	return result;
}

// Symmetric orthographic projection
//NOTE: assumes left = -right and bottom = -top
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT, 
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto ortho_sym(T right, T top, T near, T far)
{
	//TODO:error handling with assert and [[unlikely]]

	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix::identity() };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	const T inv_r{ one / right };
	const T inv_t{ one / top };
	const T inv_fn{ one / (far - near) };

	// 1. Common diagonal elements (scaling components)
	result[0, 0] = two * inv_r;	// X scaling
	result[1, 1] = two * inv_t;	// Y scaling

	// Z scaling (depends both on target range and handedness)
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = two * inv_fn;
		} else {
			result[2, 2] = static_cast<T>(-2) * inv_fn;
		}
	} else {																			 // Range: [0, 1]
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = one * inv_fn;
		} else {
			result[2, 2] = static_cast<T>(-1) * inv_fn;
		}
	}

	// 2. Non-diagonal elements (only Z translation component)
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = -(far + near) * inv_fn;
	} else {																			 // Range: [0, 1]
			tz = (-near) * inv_fn;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[2, 3] = tz;
	} else {
		result[3, 2] = tz;
	}

	return result;
}

//----------------------------------------------------------------------------------
// Perspective Projection
//----------------------------------------------------------------------------------

// Standard perspective projection
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT, 
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto persp(T left, T right, T bottom, T top, T near, T far)
{
	//TODO:error handling with assert and [[unlikely]]

	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix(static_cast<T>(0)) };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	const T inv_rl{ one / (right - left) };
	const T inv_tb{ one / (top - bottom) };
	const T inv_fn{ one / (far - near) };

	// 1. Common diagonal elements (scaling components)
	result[0, 0] = two * near * inv_rl;	// X scaling
	result[1, 1] = two * near * inv_tb;	// Y scaling
	
	// Z scaling (depends both on target range and handedness)
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = (far + near) * inv_fn;
		} else {
			result[2, 2] = -(far + near) * inv_fn;
		}
	} else {																			 // Range: [0, 1]
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = far * inv_fn;
		} else {
			result[2, 2] = -far * inv_fn;
		}
	}

	// 2. Non-diagonal elements (Translation components)
	auto tx{ (right + left) * inv_rl };
	auto ty{ (top + bottom) * inv_tb };
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = -two * far * near * inv_fn;
	} else {																			 // Range: [0, 1]
			tz = -far * near * inv_fn;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[0, 3] = tx;
		result[1, 3] = ty;
		result[2, 3] = tz;
		
		if constexpr (IsLeftHanded)
		{
			result[3, 2] = one;
		} else {
			result[3, 2] = static_cast<T>(-1);
		}

	} else {
		result[3, 0] = tx;
		result[3, 1] = ty;
		result[3, 2] = tz;

		if constexpr (IsLeftHanded)
		{
			result[2, 3] = one;
		} else {
			result[2, 3] = static_cast<T>(-1);
		}

	}

	return result;
}

// Symmetric perspective projection from right and top coordinates
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT, 
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto persp_symm(T right, T top, T near, T far)
{
	//TODO:error handling with assert and [[unlikely]]

	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix(static_cast<T>(0)) };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	const T inv_r{ one / right };
	const T inv_t{ one / top };
	const T inv_fn{ one / (far - near) };

	// 1. Common diagonal elements (scaling components)
	result[0, 0] = near * inv_r;	// X scaling
	result[1, 1] = near * inv_t;	// Y scaling
	
	// Z scaling (depends both on target range and handedness)
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = (far + near) * inv_fn;
		} else {
			result[2, 2] = -(far + near) * inv_fn;
		}
	} else {																			 // Range: [0, 1]
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = far * inv_fn;
		} else {
			result[2, 2] = -far * inv_fn;
		}
	}

	// 2. Non-diagonal elements (Only z translation component)
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = -two * far * near * inv_fn;
	} else {																			 // Range: [0, 1]
			tz = -far * near * inv_fn;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[2, 3] = tz;
		
		if constexpr (IsLeftHanded)
		{
			result[3, 2] = one;
		} else {
			result[3, 2] = static_cast<T>(-1);
		}

	} else {
		result[3, 2] = tz;

		if constexpr (IsLeftHanded)
		{
			result[2, 3] = one;
		} else {
			result[2, 3] = static_cast<T>(-1);
		}

	}

	return result;
}

// Symmetric perspective projection from FOV and aspect ratio
//NOTE: if VerticalFOV is true, aspect_ratio should be WIDTH/HEIGHT
//			if VerticalFOV is false, aspect_ratio should be HEIGHT/WIDTH
//CONVENTION: FOV in radians
template <
	bool VerticalFOV = true,
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT, 
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto persp_symm(T fov, T aspect_ratio, T near, T far)
{
	//TODO: error handling, especially for aspect ratio
	
	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix(static_cast<T>(0)) };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	const auto inv_fn{ one / (far - near) };
	
	// 1. Common diagonal elements (scaling components)
	if constexpr (VerticalFOV)
	{
		result[1, 1] = one / fcp::math::tan(fov / two);
		result[0, 0] = result[1, 1] / aspect_ratio;
	} else {
		result[0, 0] = one / fcp::math::tan(fov / two);
		result[1, 1] = result[0, 0] / aspect_ratio;
	}
	
	// Z scaling (depends both on target range and handedness)
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = (far + near) * inv_fn;
		} else {
			result[2, 2] = -(far + near) * inv_fn;
		}
	} else {																			 // Range: [0, 1]
		if constexpr (IsLeftHanded)
		{
			result[2, 2] = far * inv_fn;
		} else {
			result[2, 2] = -far * inv_fn;
		}
	}

	// 2. Non-diagonal elements (Only z translation component)
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = -two * far * near * inv_fn;
	} else {																			 // Range: [0, 1]
			tz = -far * near * inv_fn;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[2, 3] = tz;
		
		if constexpr (IsLeftHanded)
		{
			result[3, 2] = one;
		} else {
			result[3, 2] = static_cast<T>(-1);
		}

	} else {
		result[3, 2] = tz;

		if constexpr (IsLeftHanded)
		{
			result[2, 3] = one;
		} else {
			result[2, 3] = static_cast<T>(-1);
		}

	}

	return result;
}

// Infinite perspective projection
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT, 
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto persp_inf(T left, T right, T bottom, T top, T near)
{
	//TODO:error handling with assert and [[unlikely]]

	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix(static_cast<T>(0)) };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	const T inv_rl{ one / (right - left) };
	const T inv_tb{ one / (top - bottom) };

	// 1. Common diagonal elements (scaling components)
	result[0, 0] = two * near * inv_rl;	// X scaling
	result[1, 1] = two * near * inv_tb;	// Y scaling
	
	// Z scaling (depends only on handedness)
	if constexpr (IsLeftHanded)
	{
		result[2, 2] = one;
	} else {
		result[2, 2] = static_cast<T>(-1);
	}

	// 2. Non-diagonal elements (Translation components)
	auto tx{ (right + left) * inv_rl };
	auto ty{ (top + bottom) * inv_tb };
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = static_cast<T>(-2) * near;
	} else {																			 // Range: [0, 1]
			tz = -near;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[0, 3] = tx;
		result[1, 3] = ty;
		result[2, 3] = tz;
		
		if constexpr (IsLeftHanded)
		{
			result[3, 2] = one;
		} else {
			result[3, 2] = static_cast<T>(-1);
		}

	} else {
		result[3, 0] = tx;
		result[3, 1] = ty;
		result[3, 2] = tz;

		if constexpr (IsLeftHanded)
		{
			result[2, 3] = one;
		} else {
			result[2, 3] = static_cast<T>(-1);
		}

	}

	return result;
}

// Infinite symmetric perspective projection
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT, 
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto persp_inf_symm(T right, T top, T near)
{
	//TODO:error handling with assert and [[unlikely]]

	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix(static_cast<T>(0)) };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	const T inv_r{ one / right };
	const T inv_t{ one / top };

	// 1. Common diagonal elements (scaling components)
	result[0, 0] = near * inv_r;	// X scaling
	result[1, 1] = near * inv_t;	// Y scaling
	
	// Z scaling (depends only on handedness)
	if constexpr (IsLeftHanded)
	{
		result[2, 2] = one;
	} else {
		result[2, 2] = static_cast<T>(-1);
	}

	// 2. Non-diagonal elements (Only z translation component)
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = static_cast<T>(-2) * near;
	} else {																			 // Range: [0, 1]
			tz = -near;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[2, 3] = tz;
		
		if constexpr (IsLeftHanded)
		{
			result[3, 2] = one;
		} else {
			result[3, 2] = static_cast<T>(-1);
		}

	} else {
		result[3, 2] = tz;

		if constexpr (IsLeftHanded)
		{
			result[2, 3] = one;
		} else {
			result[2, 3] = static_cast<T>(-1);
		}

	}

	return result;
}

// Infinite symmetric perspective projection using FOV and aspect ratio
//NOTE: if VerticalFOV is true, aspect_ratio should be WIDTH/HEIGHT
//			if VerticalFOV is false, aspect_ratio should be HEIGHT/WIDTH
//CONVENTION: FOV in radians
template <
	bool VerticalFOV = true,
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	DepthInterval D = FCPMG_DEFAULT_DEPTH_INT, 
	typename T,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<T, 4, 4>
>
constexpr auto persp_inf_symm(T fov, T aspect_ratio, T top, T near)
{
	//TODO:error handling with assert and [[unlikely]]

	using mtraits = internal::Traits<std::remove_cvref_t<Matrix>>;

	auto result{ Matrix(static_cast<T>(0)) };

	constexpr auto one{ static_cast<T>(1) };
	constexpr auto two{ static_cast<T>(2) };

	// 1. Common diagonal elements (scaling components)
	if constexpr (VerticalFOV)
	{
		result[1, 1] = one / fcp::math::tan(fov / two);
		result[0, 0] = result[1, 1] / aspect_ratio;
	} else {
		result[0, 0] = one / fcp::math::tan(fov / two);
		result[1, 1] = result[0, 0] / aspect_ratio;
	}
	
	// Z scaling (depends only on handedness)
	if constexpr (IsLeftHanded)
	{
		result[2, 2] = one;
	} else {
		result[2, 2] = static_cast<T>(-1);
	}

	// 2. Non-diagonal elements (Translation components)
	T tz;

	// Z translation depends on target range, not on handedness
	if constexpr (D == DepthInterval::NegOneToOne) // Range: [-1, +1]
	{
			tz = static_cast<T>(-2) * near;
	} else {																			 // Range: [0, 1]
			tz = -near;
	}

	// Store based on vector convention
	if constexpr (ColumnVectorFormat)
	{
		result[2, 3] = tz;
		
		if constexpr (IsLeftHanded)
		{
			result[3, 2] = one;
		} else {
			result[3, 2] = static_cast<T>(-1);
		}

	} else {
		result[3, 2] = tz;

		if constexpr (IsLeftHanded)
		{
			result[2, 3] = one;
		} else {
			result[2, 3] = static_cast<T>(-1);
		}

	}

	return result;
}

//----------------------------------------------------------------------------------
// LookAt Matrix
//----------------------------------------------------------------------------------

// Compute LookAt matrix
//NOTE: defaults to LH, column-major matrix
template <
	bool IsLeftHanded = FCPM_GRAPHICS_USE_LHS_SYSTEM, 
	LazyVectorLike EyeVector,
	LazyVectorLike AtVector,
	LazyVectorLike UpVector,
	Lazy4x4MatrixType Matrix = DefaultMatrixType<
		typename internal::Traits<std::remove_cvref_t<EyeVector>>::element_type, 4, 4>
	>
constexpr auto look_at(const EyeVector& eye, const AtVector& at, const UpVector& world_up) 
{
	using vtraits = internal::Traits<std::remove_cvref_t<EyeVector>>;
	using T = vtraits::element_type;
	enum { X, Y, Z };

	constexpr auto zero{ static_cast<T>(0) };

	const auto fwd{ normalize(at - eye) };
	const auto right{ cross(world_up, fwd) };
	
	// If cross product is zero, use fallback vector
	bool parallel{ false };
	for (int i{0}; i < 3; i++)
		if (fcp::math::cmp(right[i], zero)) parallel = true;

	if (parallel) right = typename vtraits::materialized_type{zero, zero, static_cast<T>(1)};

	const auto nup{ cross(fwd, right) };

	const auto dre{ -dot(right, eye) };
	const auto due{ -dot(nup, eye) };

	if constexpr (vtraits::is_row_major) // Row-Vector system
	{
		if constexpr (IsLeftHanded)	// Left-Handed system
		{
			const auto dfe{ -dot(fwd, eye) };

			return Matrix{
				right[X], nup[X], fwd[X], zero,
				right[Y], nup[Y], fwd[Y], zero,
				right[Z], nup[Z], fwd[Z], zero,
				dre, due, dfe, static_cast<T>(1)	
			};	
		} else {										// Right-Handed system
			const auto dfe{ dot(fwd, eye) };

			return Matrix{
				right[X], nup[X], -fwd[X], zero,
				right[Y], nup[Y], -fwd[Y], zero,
				right[Z], nup[Z], -fwd[Z], zero,
				dre, due, dfe, static_cast<T>(1)	
			};	
		}
	} else {														// Column-Vector system
		if constexpr (IsLeftHanded)	// Left-Handed system
		{
			const auto dfe{ -dot(fwd, eye) };

			return Matrix{
				right[X], right[Y], right[Z], dre,
				nup[X],   nup[Y],   nup[Z],   due,
				fwd[X],   fwd[Y],   fwd[Z],   dfe,
				zero,     zero,     zero,     static_cast<T>(1)	
			};	
		} else {										// Right-Handed system
			const auto dfe{ dot(fwd, eye) };

			return Matrix{
				right[X],  right[Y], right[Z], dre,
				nup[X],    nup[Y],   nup[Z],   due,
				-fwd[X],  -fwd[Y],  -fwd[Z],   dfe,
				zero,     zero,     zero,     static_cast<T>(1)	
			};	
		}
	}
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_SPACE_CONVERSIONS_HPP
