#ifndef FCP_MATH_CORE_MATH_TRIG_HPP
#define FCP_MATH_CORE_MATH_TRIG_HPP

#include "core/internal/common.hpp"

#include <cmath>
#include <limits>
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// TODO: use CORDIC or MINIMAX
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Sine function
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

//----------------------------------------------------------------------------------
// Cosine function
//----------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------
// Tangent function
//----------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------
// Arccosine function
//----------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------
// Arcsine function
//----------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------
// Arctangent-2 function
//----------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------
// Arctangent function
//----------------------------------------------------------------------------------

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

#endif	//FCP_MATH_CORE_MATH_TRIG_HPP 
