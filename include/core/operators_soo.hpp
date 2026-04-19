#ifndef FCP_MATH_CORE_OPERATORS_SOO_HPP
#define FCP_MATH_CORE_OPERATORS_SOO_HPP

#include "core/constexpr_statements.hpp"
#include "core/internal/common.hpp"

#include "linalg/internal/scalar.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_OPERATORS_NAMESPACE

//----------------------------------------------------------------------------------
// Catch-all templates
//----------------------------------------------------------------------------------

//----------------------------------------------------
// Arithmetic operators
//----------------------------------------------------

template <typename L, typename R>																				
	requires (LazyType<L> || LazyType<R>) &&
					 SmallLazyType<internal::get_max_traits_t<L, R>>
constexpr auto operator+(L&& left, R&& right)													
{																																				
	// Get the type that results from a call to wrap_scalar in						
	// order to deduce the underlying type, while													
	// keeping the reference level (for "heavy" objects passed						
	// by reference)																											
	using lwt = decltype(wrap_scalar(left));															
	using rwt = decltype(wrap_scalar(right));															
																																				
	// Deduce the underlying type																					
	using T = decltype(lwt(left).evaluate(0) + rwt(right).evaluate(0));		
	
	// Inject the results of the function call directly 									
	// This way I don't have to manage generic code that has							
	// to be a reference for some types and a copy for others 						
	// (i.e. the "heavy" ones)																						
	//																																		
	// Note that this way there are two main possibilities:								
	// 1. A scalar literal/variable was passed and wrap_scalar 						
	// 		will pass its wrapper by value to SumExpr												
	// 2. A "heavy" object was passed and wrap_scalar will just						
	// 		forward the reference to the constructor of SumExpr							
	//																																		
	//NOTE: this is the reason why "heavy" objects need to be 						
	//			defined as static if one wishes to use them in 		 						
	//			compile-time evaluation																				

	//TODO: SOO
	using lraw_t = std::remove_cvref_t<lwt>;
	using rraw_t = std::remove_cvref_t<rwt>;
	using max_traits = internal::get_max_traits_t<lraw_t, rraw_t>;

	typename max_traits::materialized_type result;	

	for_constexpr<0, max_traits::rows>([&](auto i) {
		for_constexpr<0, max_traits::columns>([&](auto j) {
			result[i, j] = left[i, j] + right[i, j];
		});
	});

	//for (int i{0}; i < max_traits::rows; i++)
	//	for (int j{0}; j < max_traits::columns; j++)
	//		result[i, j] = left[i, j] + right[i, j];

	return result;
}

END_FCP_OPERATORS_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_OPERATORS_SOO_HPP
