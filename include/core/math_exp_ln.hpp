#ifndef FCP_MATH_CORE_MATH_EXPLN_HPP
#define FCP_MATH_CORE_MATH_EXPLN_HPP

#include "core/internal/common.hpp"

#include <cmath>
#include <limits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Exponentiation
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
// Taylor series
template <typename T>
consteval T exp_impl(T x)
{
	using lim = std::numeric_limits<T>;

	if (x < 0) return static_cast<T>(1) / exp_impl(-x);

	T sum{ static_cast<T>(1) };	
	T term{ static_cast<T>(1) };	

	for (int n{1}; n < 100; n++)
	{
		term *= x / n;
		sum += term;

		if (term < (lim::min() * lim::epsilon())) break;
	}

	return sum;
}
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T exp(T x)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;

	if consteval {
		return static_cast<T>(
				internal::exp_impl<F>(static_cast<F>(x))
		);
	} else {
		return std::exp(x);
	}
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE



#endif	//FCP_MATH_CORE_MATH_EXPLN_HPP
