#ifndef FCP_MATH_CORE_MATH_POWROOT_HPP
#define FCP_MATH_CORE_MATH_POWROOT_HPP

#include "core/internal/common.hpp"
#include "core/math_comparison.hpp"

#include <cmath>
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Power function
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
template <typename T>
constexpr T pow_impl(T base, T exp)
{
	if (cmp(base, static_cast<T>(0)) && exp < static_cast<T>(0)) 
		return std::numeric_limits<T>::quiet_NaN();
	if (
			cmp(base, static_cast<T>(0)) || cmp(base, static_cast<T>(1)) || 
			cmp(exp, static_cast<T>(1))
		 ) 
		return base;
	if (cmp(exp, static_cast<T>(0))) return static_cast<T>(1);
	if (exp < 0) return static_cast<T>(1) / pow_impl(base, -exp);

	T result{ static_cast<T>(1) };

	for (T i{static_cast<T>(0)}; i < exp; i++)
		result *= base;
	
	return result;
}
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T pow(T base, T exp)
{
	if consteval {
		return internal::pow_impl(base, exp);
	} else {
		return std::pow(base, exp);
	}
}

//----------------------------------------------------------------------------------
// N-th root
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
// Heron's method / Newton-Raphon's method
template <typename T>
consteval T n_root_impl(T x, int n)
{
	if (x < static_cast<T>(0)) return std::numeric_limits<T>::quiet_NaN();
	if (cmp(x, static_cast<T>(0)) || cmp(x, static_cast<T>(1))) return x;

	T current{static_cast<T>(x)}, previous{static_cast<T>(0)};

	const T n_inv{ static_cast<T>(1) / static_cast<T>(n) };
	const int nmo{ n - 1 };

	int counter{0}; 
	while (counter++ < 100)
	{
		previous = current;
		current = n_inv * (nmo*previous + (x / previous));

		if (cmp(current, previous)) break;
	}

	return current;
}
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T n_root(T x, int p)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;

	if consteval {
		return static_cast<T>(internal::n_root_impl<F>(x, p));
	} else {
		return static_cast<T>(
				std::pow<F>(x, static_cast<F>(1)/static_cast<F>(p))
		);
	}
}

// Convenient specialization of n_root
template <LazyScalarLike T>
constexpr T sqrt(T x)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;

	if consteval {
		return static_cast<T>(
				internal::n_root_impl<F>(x, 2)
		);
	} else {
		return std::sqrt(x);
	}
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_MATH_POWROOT_HPP
