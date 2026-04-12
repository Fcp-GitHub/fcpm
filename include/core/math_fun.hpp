#ifndef FCP_MATH_CORE_MATHFUN_HPP
#define FCP_MATH_CORE_MATHFUN_HPP

#include "core/common.hpp"
#include "core/hardware.hpp"
#include "core/float_traits.hpp"

#include <cmath>
#include <bit>
#include <limits>
#include <concepts>
#include <stdfloat>

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
// Power function
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
template <typename T>
constexpr T pow_impl(T base, T exp)
{
	constexpr auto zero{ static_cast<T>(0) };
	constexpr auto one{ static_cast<T>(1) };

	if (cmp(base, zero) && exp < zero) return std::numeric_limits<T>::quiet_NaN();
	if (cmp(base, zero) || cmp(base, one)|| cmp(exp, one)) return base;
	if (cmp(exp, zero)) return one;
	if (exp < 0) return one / pow_impl(base, -exp);

	T result{ one };

	for (T i{zero}; i < exp; i++)
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
	constexpr auto zero{ static_cast<T>(0) };
	constexpr auto one{ static_cast<T>(1) };

	if (x < zero) return std::numeric_limits<T>::quiet_NaN();
	if (x == zero || x == one) return x;

	T current{static_cast<T>(x)}, previous{zero};

	const T n_inv{ one / static_cast<T>(n) };
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

//----------------------------------------------------------------------------------
// Clamp
//----------------------------------------------------------------------------------
template <LazyScalarLike T>
constexpr T clamp(T x, T min, T max)
{
	const T temp{ x < min ? min : x };
	return x > max ? max : x;
}

//----------------------------------------------------------------------------------
// Trig functions
// TODO: use CORDIC or MINIMAX
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
// Taylor series
template <typename T>
consteval T sin_impl(T x)
{
	using lim = std::numeric_limits<T>;

	if (x < 0) return -sin_impl(-x);

	T sum{ static_cast<T>(0) };	
	T term{ static_cast<T>(1) };	
	T one{ static_cast<T>(-1) };

	for (int n{1}; n < 10; n++)
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
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T sin(T x)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;

	if consteval {
		return static_cast<T>(internal::sin_impl<F>(x));
	} else {
		return static_cast<T>(std::sin(x));
	}
}

START_FCP_INTERNAL_NAMESPACE
// Taylor series
template <typename T>
consteval T cos_impl(T x)
{
	using lim = std::numeric_limits<T>;

	if (x < 0) return cos_impl(-x);

	T sum{ static_cast<T>(1) };	
	T term{ static_cast<T>(1) };	
	T one{ static_cast<T>(1) };

	for (int n{1}; n < 10; n++)
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
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T cos(T x)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;

	if consteval {
		return static_cast<T>(internal::cos_impl<F>(x));
	} else {
		return static_cast<T>(std::cos(x));
	}
}

template <LazyScalarLike T>
constexpr T tan(T x)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;

	if consteval {
		return static_cast<T>(sin<F>(x) / cos<F>(x));
	} else {
		return static_cast<T>(std::tan(x));
	}
}

START_FCP_INTERNAL_NAMESPACE
// From: https://developer.download.nvidia.com/cg/acos.html
template <typename T>
constexpr T acos_impl(T x)
{
  T negate = static_cast<T>(x < 0);
  x = abs(x);
  T ret = -0.0187293;
  ret = ret * x;
  ret = ret + 0.0742610;
  ret = ret * x;
  ret = ret - 0.2121144;
  ret = ret * x;
  ret = ret + 1.5707288;
  ret = ret * sqrt(1.0-x);
  ret = ret - 2 * negate * ret;
  return negate * 3.14159265358979 + ret;
}
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T acos(T x)
{
	//TODO: domain check ( -1 <= x <= 1 )

	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;
	
	if consteval {
		return static_cast<T>(internal::acos_impl<F>(x));
	}
	else {
		return static_cast<T>(std::acos(x));
	}
}

START_FCP_INTERNAL_NAMESPACE
// From: https://developer.download.nvidia.com/cg/asin.html
template <typename T>
constexpr T asin_impl(T x)
{
	T negate = static_cast<T>(x < 0);
  x = abs(x);
  T ret = -0.0187293;
  ret *= x;
  ret += 0.0742610;
  ret *= x;
  ret -= 0.2121144;
  ret *= x;
  ret += 1.5707288;
  ret = 3.14159265358979*0.5 - sqrt(1.0 - x)*ret;
  return ret - 2 * negate * ret;
}
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T asin(T x)
{
	//TODO: domain check ( -1 <= x <= 1 )

	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;
	
	if consteval {
		return static_cast<T>(internal::asin_impl<F>(x));
	}
	else {
		return static_cast<T>(std::asin(x));
	}
}

START_FCP_INTERNAL_NAMESPACE
// From: https://developer.download.nvidia.com/cg/asin.html
template <typename T>
constexpr T atan2_impl(T y, T x)
{
	T t0, t1, t2, t3, t4;

  t3 = std::abs(x);
  t1 = std::abs(y);
  t0 = std::max(t3, t1);
  t1 = std::min(t3, t1);
  t3 = static_cast<T>(1) / t0;
  t3 = t1 * t3;

  t4 = t3 * t3;
  t0 =         - static_cast<T>(0.013480470);
  t0 = t0 * t4 + static_cast<T>(0.057477314);
  t0 = t0 * t4 - static_cast<T>(0.121239071);
  t0 = t0 * t4 + static_cast<T>(0.195635925);
  t0 = t0 * t4 - static_cast<T>(0.332994597);
  t0 = t0 * t4 + static_cast<T>(0.999995630);
  t3 = t0 * t3;

  t3 = (abs(y) > abs(x)) ? static_cast<T>(1.570796327) - t3 : t3;
  t3 = (x < 0) ?  static_cast<T>(3.141592654) - t3 : t3;
  t3 = (y < 0) ? -t3 : t3;

  return t3;
}
END_FCP_INTERNAL_NAMESPACE

template <LazyScalarLike T>
constexpr T atan2(T y, T x)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;
	
	if consteval {
		return static_cast<T>(internal::atan2_impl<F>(y, x));
	}
	else {
		return static_cast<T>(std::atan2(y, x));
	}
}

template <LazyScalarLike T>
constexpr T atan(T x)
{
	using F = std::conditional_t<
		std::is_floating_point_v<T>,
		T, double
	>;
	
	if consteval {
		return static_cast<T>(internal::atan2_impl<F>(x, 1));
	}
	else {
		return static_cast<T>(std::atan(x));
	}
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_MATHFUN_HPP
