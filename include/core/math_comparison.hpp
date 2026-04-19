#ifndef FCP_MATH_CORE_MATH_CMP_HPP
#define FCP_MATH_CORE_MATH_CMP_HPP

#include "core/internal/common.hpp"
#include "core/internal/float_traits.hpp"

#include <cmath>
#include <limits>
#include <bit>
#include <concepts>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Equality comparison
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE

// Fast compare
// Useful for graphics programming where performance is more 
// needed then perfect accuracy
template <std::floating_point T>
constexpr bool cmp_fast(T x, T y)
{
	const T abs_diff{ (x > y) ? (x - y) : (y - x) };
	return abs_diff < (2.L * FCPM_CMP_EPSILON);
}

// Accurate comparison using:
// - absolute difference for comparisons against zero
// - ULPs for other comparisons
template <std::floating_point T>
constexpr bool cmp_accurate(T x, T y)
{
	using lim = std::numeric_limits<T>;
	using traits = internal::FloatTraits<T>;
	using eq_t = traits::eq_int_t;
	
	constexpr T zero{ static_cast<T>(0) };

	// Check for different signs
	if ((std::bit_cast<eq_t>(x) < 0) != (std::bit_cast<eq_t>(y) < 0))
	{
		// Make sure +0 == -0
		if (x == y) return true;
		return false;
	}

	// Handle comparisons against zero
	// using absolute difference
	if (x == zero || y == zero)
	{
		T diff{ (x > y) ? (x - y) : (y - x) };
		return diff < (lim::epsilon() * lim::min());
	}

	// Handle comparisons against non-zero
	// using ULPs
	const auto ULPs_diff{
		(x > y) ? (std::bit_cast<eq_t>(x) - std::bit_cast<eq_t>(y)) : (std::bit_cast<eq_t>(y) - std::bit_cast<eq_t>(x))
	};	

	return ULPs_diff <= FCPM_CMP_MAX_ULP;
}

END_FCP_INTERNAL_NAMESPACE

// Compare two floating-point numbers using the specified 
// equality test
template <std::floating_point T, int Method = FCPM_CMP_INTERNAL>
constexpr bool cmp(T x, T y)
{
	if constexpr (Method == FCPM_CMP_FAST)
		return internal::cmp_fast(x, y);
	else
		return internal::cmp_accurate(x, y);
}

// Compare two integers
template <typename T>
	requires (!std::floating_point<T>)
constexpr bool cmp(T x, T y)
{
	return x == y;
}

//----------------------------------------------------------------------------------
// Floating-point relational tests
//----------------------------------------------------------------------------------

template <typename T>
constexpr bool less_or_equal(T x, T y)
{
	return (x < y) or cmp(x, y);
}

template <typename T>
constexpr bool greater_or_equal(T x, T y)
{
	return (x > y) or cmp(x, y);
}

//----------------------------------------------------------------------------------
// Absolute value
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
template <typename T>
constexpr T abs_impl(T x)
{
	return (x >= 0) ? x : -x;
}
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T abs(T x)
{
	if consteval {
		return internal::abs_impl(x);
	} else {
		return std::abs(x);
	}
}

//----------------------------------------------------------------------------------
// Clamp
//----------------------------------------------------------------------------------
template <LazyScalarLike T>
constexpr T clamp(T x, T min, T max)
{
	const T temp{ x < min ? min : x };
	return x > max ? max : x;
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_MATH_CMP_HPP
