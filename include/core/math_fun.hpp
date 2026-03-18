#ifndef FCP_MATH_CORE_MATHFUN_HPP
#define FCP_MATH_CORE_MATHFUN_HPP

#include "core/base.hpp"

#include <cmath>
#include <limits>
#include <concepts>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE


//----------------------------------------------------------------------------------
// Floating-point equality test
//----------------------------------------------------------------------------------
//TODO
template <std::floating_point T>
constexpr bool cmp(T x, T y)
{
	using lim = std::numeric_limits<T>;

	// Get absolute difference
	T diff{ (x > y) ? (x - y) : (y - x) };

	// Handle values near zero
	if (x == 0 || y == 0 || diff < lim::min())
		return diff < (lim::epsilon() * lim::min());

	// Relative difference
	T abs_x{ (x < 0) ? (-x) : x };
	T abs_y{ (y < 0) ? (-y) : y };
	T max_val{ (abs_x > abs_y) ? abs_x : abs_y };

	return diff < (max_val * lim::epsilon());
}

template <typename T>
constexpr bool cmp(T x, T y)
{
	return x == y;
}

//----------------------------------------------------------------------------------
// Absolute value
//----------------------------------------------------------------------------------

template <typename T>
constexpr T abs_impl(T x)
{
	return (x >= 0) ? x : -x;
}

template <typename T>
constexpr T abs(T x)
{
	if consteval {
		return abs_impl(x);
	} else {
		return std::abs(x);
	}
}

//----------------------------------------------------------------------------------
// Power function
//----------------------------------------------------------------------------------

template <typename T>
consteval T pow_impl(T base, T exp)
{
	if (base == 0 && exp < 0) return std::numeric_limits<T>::quiet_NaN();
	if (base == 0 || base == 1) return base;
	if (exp == 1) return static_cast<T>(1);
	if (exp < 0) return static_cast<T>(1) / pow_impl(base, -exp);

	T result{ base };

	for (int i{0}; i < exp; i++)
		result *= base;
	
	return result;
}

template <typename T>
constexpr T pow(T base, T exp)
{
	if consteval {
		pow_impl(base, exp);
	} else {
		std::pow(base, exp);
	}
}

//----------------------------------------------------------------------------------
// Square root
//----------------------------------------------------------------------------------

// Heron's method / Newton-Raphon's method
template <typename T>
consteval T sqrt_impl(T x)
{
	if (x < 0) return std::numeric_limits<T>::quiet_NaN();
	if (x == 0 || x == 1) return x;

	T current{x}, previous{0};

	int counter{0}; 
	while (counter++ < 100)
	{
		previous = current;
		current = 0.5 * (previous + (x / previous));

		if (cmp(current, previous)) break;
	}

	return current;
}

template <typename T>
constexpr T sqrt(T x)
{
	if consteval {
		return sqrt_impl(x);
	} else {
		return std::sqrt(x);
	}
}

//----------------------------------------------------------------------------------
// Exponentiation
//----------------------------------------------------------------------------------

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

template <typename T>
constexpr T exp(T x)
{
	if consteval {
		return exp_impl(x);
	} else {
		return std::exp(x);
	}
}

//----------------------------------------------------------------------------------
// Clamp
//----------------------------------------------------------------------------------
template <typename T>
constexpr T clamp(T x, T min, T max)
{
	const T temp{ x < min ? min : x };
	return x > max ? max : x;
}

//----------------------------------------------------------------------------------
// Trig functions
// TODO: use CORDIC
//----------------------------------------------------------------------------------

// Taylor series
template <typename T>
consteval T sin_impl(T x)
{
	using lim = std::numeric_limits<T>;

	if (x < 0) return -sin_impl(-x);

	T sum{ static_cast<T>(0) };	
	T term{ static_cast<T>(1) };	
	T one{ static_cast<T>(-1) };

	for (int n{1}; n < 200; n++)
	{
		if (n % 2 == 0)
		{
			term *= x / n;	
		} else {
			one *= static_cast<T>(-1);
			term *= one * x / n;
			sum += term;
		}

		if (term < (lim::min() * lim::epsilon())) break;
	}

	return sum;
}

template <typename T>
constexpr T sin(T x)
{
	if consteval {
		return sin(x);
	} else {
		return std::sin(x);
	}
}

// Taylor series
template <typename T>
consteval T cos_impl(T x)
{
	using lim = std::numeric_limits<T>;

	if (x < 0) return cos_impl(-x);

	T sum{ static_cast<T>(1) };	
	T term{ static_cast<T>(1) };	
	T one{ static_cast<T>(1) };

	for (int n{1}; n < 200; n++)
	{
		if (n % 2 == 0)
		{
			one *= static_cast<T>(-1);
			term *= one * x / n;
			sum += term;
		} else {
			term *= x / n;
		}

		if (term < (lim::min() * lim::epsilon())) break;
	}

	return sum;
}

template <typename T>
constexpr T cos(T x)
{
	if consteval {
		return cos(x);
	} else {
		return std::cos(x);
	}
}

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_MATHFUN_HPP
