#ifndef FCP_MATH_H_LAZY_OPERATORS_HPP
#define FCP_MATH_H_LAZY_OPERATORS_HPP

#include <iostream>

#include "core/base.hpp"
#include "internal/expression_templates.hpp"
#include "linalg/scalar.hpp"

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_OPERATORS_NAMESPACE

//----------------------------------------------------------------------------------
// Overloads of trigger operators
//----------------------------------------------------------------------------------

// Some templates catch "all" possibilities, meaning that every combination of
// vector/expression/scalar sum that can be lazy-evaluated is defined.
// Some other templates are have more constraints. For example, operator*()
// only implements vector-scalar multiplication, leaving dot and cross product
// operations to dedicated functions.

//NOTE: since the classes marked as "heavy" are passed around by
//      reference, they have to be defined as static if compile-
//      time computations are needed.

//----------------------------------------------------------------------------------
// Catch-all templates
//----------------------------------------------------------------------------------


//----------------------------------------------------
// Arithmetic operators
//----------------------------------------------------

template <typename L, typename R>																				
	requires LazyType<L> || LazyType<R>																		
constexpr auto operator+(L&& left, R&& right)													
{																																				
	//TODO: I'm probably typecasting too much between these operators			
	//      and the Expression classes (subexpr_t trait)									
																																				
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
	return internal::SumExpr<lwt, rwt, T>(																
			wrap_scalar(left),																								
		 	wrap_scalar(right)																								
	);																																		
}	

// Subtraction
template <typename L, typename R>
	requires LazyType<L> || LazyType<R>
constexpr auto operator-(L&& left, R&& right)
{
	using lwt = decltype(wrap_scalar(left));
	using rwt = decltype(wrap_scalar(right));

	// Deduce underlying type
	using T = decltype(lwt(left).evaluate(0) - rwt(right).evaluate(0));

	return internal::SubExpr<lwt, rwt, T>(
			wrap_scalar(left),
			wrap_scalar(right)
	);
}

template <typename E>
	requires LazyType<E>
constexpr auto operator+(E&& expr)
{
	using ewt = decltype(wrap_scalar(expr));

	using T = decltype(ewt(expr).evaluate(0));
	
	return internal::IdeExpr<ewt, T>(
		wrap_scalar(expr)
	);
}

template <typename E>
	requires LazyType<E>
constexpr auto operator-(E&& expr)
{
	using ewt = decltype(wrap_scalar(expr));

	using T = decltype(ewt(expr).evaluate(0));

	return internal::NegExpr<ewt, T>(
		wrap_scalar(expr)
	);
}

//----------------------------------------------------
// Pretty-print overload
//----------------------------------------------------

template <typename E>
	requires LazyType<E>
std::ostream& operator<<(std::ostream& out, const E& expr)
{
	/*constexpr*/ auto dim = expr.size();
	out << '(';
	for (std::size_t i{0}; i < dim - 1; i++)
		out << expr[i] << ", ";
	out << expr[dim - 1] << ')';

	return out;
}

//----------------------------------------------------------------------------------
// Constraint templates
//----------------------------------------------------------------------------------

// Multiplication (matrix-scalar)
template <typename L, typename R>
	requires (LazyType<L> || LazyType<R>) &&
					 (!(LazyMatrixLike<L> && LazyMatrixLike<R>))
constexpr auto operator*(L&& left, R&& right)
{
	using lwt = decltype(wrap_scalar(left));
	using rwt = decltype(wrap_scalar(right));

	// Deduce underlying type
	using T = decltype(lwt(left).evaluate(0) * rwt(right).evaluate(0));

	return internal::MulExpr<lwt, rwt, T>(
			wrap_scalar(left),
			wrap_scalar(right)
	);
}
		
// Division (matrix-scalar)
template <typename L, typename R>
	requires (LazyType<L> || LazyType<R>) &&
					 (!(LazyMatrixLike<L> && LazyMatrixLike<R>))
constexpr auto operator/(L&& left, R&& right)
{
	using lwt = decltype(wrap_scalar(left));
	using rwt = decltype(wrap_scalar(right));

	// Deduce underlying type
	using T = decltype(lwt(left).evaluate(0) * rwt(right).evaluate(0));

	return internal::DivExpr<lwt, rwt, T>(
			wrap_scalar(left),
			wrap_scalar(right)
	);
}

END_FCP_OPERATORS_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_H_LAZY_OPERATORS_HPP
