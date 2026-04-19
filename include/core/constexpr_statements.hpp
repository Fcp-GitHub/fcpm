#ifndef FCP_MATH_CORE_CONSTEXPR_STATEMENTS_HPP
#define FCP_MATH_CORE_CONSTEXPR_STATEMENTS_HPP

#include "core/internal/base.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

// Compile-time switch statement
template <typename T, typename... Choices>
struct match_type : std::false_type{};	// Base case

template <typename T, typename Choice, typename Result, typename... Others>
struct match_type<T, Choice, Result, Others...>
{
	using type = std::conditional_t<
		std::is_same_v<T, Choice>,	// if T matches Choice...
		Result,										 	// use Result 
		typename match_type<T, Others...>::type // else go to the next recursive step
	>;	
};

template <typename T, typename... Choices>
using match_type_t = match_type<T, Choices...>::type;

// Compile-time for loop
template <int From, int To>
constexpr void for_constexpr(auto&& body)
{
	if constexpr (From < To)
	{
		// Pass the index to the loop body
		body(std::integral_constant<int, From>{});

		// Recursively go to the next index
		//for_constexpr<From+1, To>(std::forward<decltype(body)>(body));
		for_constexpr<From+1, To>(static_cast<decltype(body)>(body));
	}
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_CONSTEXPR_STATEMENTS_HPP
