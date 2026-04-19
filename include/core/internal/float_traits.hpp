#ifndef FCP_MATH_CORE_FLOAT_TRAITS_HPP
#define FCP_MATH_CORE_FLOAT_TRAITS_HPP

#include "core/internal/base.hpp"

#include <stdfloat>
#include <cstdint>
#include <concepts>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

template <std::floating_point T>
struct FloatTraits;

template <>
struct FloatTraits<float>
{
	using eq_int_t = std::int32_t; 
	
	constexpr static int mantissa{ 23 };
	constexpr static int exponent{ 8 };
};

template <>
struct FloatTraits<double>
{
	using eq_int_t = std::int64_t; 
	
	constexpr static int mantissa{ 52 };
	constexpr static int exponent{ 11 };
};

//TODO
//template <>
//struct FloatTraits<long double>;

#ifdef __STDCPP_FLOAT16_T__	
template <>
struct FloatTraits<std::float16_t>
{
	using eq_int_t = std::int16_t;

	constexpr static int mantissa{ 10 };
	constexpr static int exponent{ 5 };
};
#endif

#ifdef __STDCPP_FLOAT32_T__
template <>
struct FloatTraits<std::float32_t>
{
	using eq_int_t = std::int32_t; 
	
	constexpr static int mantissa{ 23 };
	constexpr static int exponent{ 8 };
};
#endif

#ifdef __STDCPP_FLOAT64_T__
template <>
struct FloatTraits<std::float64_t>
{
	using eq_int_t = std::int64_t; 
	
	constexpr static int mantissa{ 52 };
	constexpr static int exponent{ 11 };
};
#endif

//TODO
//#ifdef __STDCPP_FLOAT128_T__
//template <>
//struct FloatTraits<std::float128_t>;
//#endif

//TODO
//#ifdef __STDCPP_BFLOAT16_T__
//template <>
//struct FloatTraits<std::bfloat16_t>
//{
//	using eq_int_t = std::int16_t;
//
//	constexpr static int mantissa{ 7 };
//	constexpr static int exponent{ 8 };
//};
//#endif


END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_FLOAT_TRAITS_HPP 
