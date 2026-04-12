#ifndef FCP_TESTS_UTILITY_FLOAT_HPP
#define FCP_TESTS_UTILITY_FLOAT_HPP

#include "core/float_traits.hpp"

#include <bit>
#include <concepts>
#include <type_traits>

namespace utility
{
	// Compute ULPs between two floating-point numbers
	template <std::floating_point T>
	constexpr auto get_ulps(T x, T y)
	{
		using eq_t = fcp::math::internal::FloatTraits<std::remove_cvref_t<T>>::eq_int_t;

		const auto diff{
			(x > y) ? (std::bit_cast<eq_t>(x) - std::bit_cast<eq_t>(y)) : 
								(std::bit_cast<eq_t>(y) - std::bit_cast<eq_t>(x))
		};	

		return diff;
	}

}	// namespace utility

#endif	//FCP_TESTS_UTILITY_FLOAT_HPP
