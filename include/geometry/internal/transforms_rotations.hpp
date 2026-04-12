#ifndef FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_ROTATIONS_HPP
#define FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_ROTATIONS_HPP

#include "core/common.hpp"
#include "core/forward.hpp"

#include "core/hardware.hpp"
#include "linalg/matrix.hpp"

#include "geometry/internal/quaternion_class.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Rotation with quaternions
//----------------------------------------------------------------------------------

// Rotate vector v using a unit quaternion
template <LazyUnitQuaternionLike Q, Lazy3DVectorLike Vector>
constexpr auto rotate(const Vector& v, const Q& q)
{
	using qtraits = internal::Traits<std::remove_cvref_t<Q>>;
	using T = qtraits::element_type;

	const auto w{ q.scalar() };
	const auto im{ q.vector() };

	const auto temp{ static_cast<T>(2) * cross(im, v) };

	return (v + (temp * w) + cross(im, temp)).eval();
}

// Rotate vector v using a generic quaternion
template <LazyQuaternionLike Q, Lazy3DVectorLike Vector>
	requires (!LazyUnitQuaternionLike<Q>)
constexpr auto rotate(const Vector& v, const Q& q)
{
	using qtraits = internal::Traits<std::remove_cvref_t<Q>>;
	using T = qtraits::element_type;

	Quaternion<T> temp{ static_cast<T>(0), v };

	return (q * temp * q.inverse()).vector();
}

//----------------------------------------------------------------------------------
// Rotation with matrices
//----------------------------------------------------------------------------------

// Perform rotation of angle `angle` around generic vector `n`
//NOTE: returns a column-major matrix by default
//CONVENTION: angle is in radians
template <
	bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT,
	typename T, 
	Lazy3DVectorLike Vector, 
	Lazy4x4MatrixLike Matrix = ColMatrix<T, 4, 4>
>
constexpr auto rotate(const Matrix& m, const T angle, const Vector& n)
{
	using vtraits = internal::Traits<std::remove_cvref_t<Vector>>;
	enum { X, Y, Z };

	const auto cos{ fcp::math::cos(angle) };
	const auto sin{ fcp::math::sin(angle) };

	const auto temp{ 1 - cos };
	constexpr auto zero{ static_cast<T>(0) };
	
	if constexpr (ColumnVectorFormat)
	{
		return (m * Matrix{
			temp*n[X]*n[X] + cos, temp*n[X]*n[Y] - n[Z]*sin, temp*n[X]*n[Z] + n[Y]*sin, zero,	
			temp*n[Y]*n[X] + n[Z]*sin, temp*n[Y]*n[Y] + cos, temp*n[Y]*n[Z] - n[X]*sin, zero,
			temp*n[Z]*n[X] - n[Y]*sin, temp*n[Z]*n[Y] + n[X]*sin, temp*n[Z]*n[Z] + cos, zero,
			zero, zero, zero, static_cast<T>(1)
		}).eval();
	} else {
		return (m * Matrix{
			temp*n[X]*n[X] + cos, temp*n[X]*n[Y] + n[Z]*sin, temp*n[X]*n[Z] - n[Y]*sin, zero,	
			temp*n[Y]*n[X] - n[Z]*sin, temp*n[Y]*n[Y] + cos, temp*n[Y]*n[Z] + n[X]*sin, zero,
			temp*n[Z]*n[X] + n[Y]*sin, temp*n[Z]*n[Y] - n[X]*sin, temp*n[Z]*n[Z] + cos, zero,
			zero, zero, zero, static_cast<T>(1)
		}).eval();
	}
}

//----------------------------------------------------------------------------------
// Rotation with Euler Angles
//----------------------------------------------------------------------------------

// Rotate vector specifying a set of Euler Angles
template <typename T, Lazy3DVectorLike Vector>
constexpr auto rotate(T heading, T pitch, T bank, const Vector& v)
{
	return rotate(v, UnitQuaternion<T>::from_euler(heading, pitch, bank));
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_GEOMETRY_INTERNAL_TRANSFORMS_ROTATIONS_HPP
