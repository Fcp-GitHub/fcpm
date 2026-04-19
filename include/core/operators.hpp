#ifndef FCP_MATH_H_LAZY_OPERATORS_HPP
#define FCP_MATH_H_LAZY_OPERATORS_HPP

#include "core/internal/common.hpp"
#include "core/internal/expression_templates.hpp"

#include "linalg/internal/scalar.hpp"

//#include "core/operators_soo.hpp"	// IWYU pragma: keep

#include <iostream>
#include <type_traits>

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
	requires (LazyType<L> || LazyType<R>) //&&
					 //LargeLazyType<internal::get_max_traits_t<L, R>>
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

	//TODO: SOO
	using lraw_t = std::remove_cvref_t<lwt>;
	using rraw_t = std::remove_cvref_t<rwt>;
	using max_traits = internal::get_max_traits_t<lraw_t, rraw_t>;

	const auto left_w{ wrap_scalar(left) };
	const auto right_w{ wrap_scalar(right) };

	if constexpr (max_traits::size <= 16)
	{
		typename max_traits::materialized_type result;	
		for (int i{0}; i < max_traits::rows; i++)
			for (int j{0}; j < max_traits::columns; j++)
				result[i, j] = left_w[i, j] + right_w[i, j];

		return result;
	} else {
		return internal::SumExpr<lwt, rwt, T>(																
				left_w,
				//static_cast<wrap_scalar_t<std::remove_cvref_t<L>>>(left),																								
				right_w
			 	//static_cast<wrap_scalar_t<std::remove_cvref_t<R>>>(right)																								
		);																																		
	}
}	

// Add-Assign, only for materialized types
template <LazyMaterializedType L, LazyMaterializedType R>
	requires (!(LazyScalarLike<L> && !LazyScalarLike<R>))
constexpr auto operator+=(L&& left, R&& right)
{
	return left = (left + right);	
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

	//TODO: SOO
	using ltraits = internal::Traits<std::remove_cvref_t<lwt>>;
	using rtraits = internal::Traits<std::remove_cvref_t<rwt>>;
	using max_traits = std::conditional_t<
		(ltraits::size > rtraits::size),
		ltraits,
		rtraits
	>;

	const auto left_w{ wrap_scalar(left) };
	const auto right_w{ wrap_scalar(right) };

	if constexpr (max_traits::size <= 16)
	{
		typename max_traits::materialized_type result;	
		for (int i{0}; i < max_traits::rows; i++)
			for (int j{0}; j < max_traits::columns; j++)
				result[i, j] = left_w[i, j] - right_w[i, j];

		return result;
	} else {
		return internal::SubExpr<lwt, rwt, T>(
				left_w,
				right_w
		);
	}
}

// Subtract-Assign, only for materialized types
template <LazyMaterializedType L, LazyMaterializedType R>
	requires (!(LazyScalarLike<L> && !LazyScalarLike<R>))
constexpr auto operator-=(L&& left, R&& right)
{
	return left = (left - right);	
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

template <LazyType M>
	requires (!LazyVectorLike<M>)
std::ostream& operator<<(std::ostream& out, const M& expr)
{
	const auto dim{ expr.size() };
	const auto rows{ expr.rows() };

	out << '(';
	for (int i{0}; i < dim - 1; i++)
		out << expr[i] << ( ((i+1)%expr.rows() == 0) ? "\n " : ", ");
	out << expr[dim - 1] << ')';

	if (!internal::Traits<std::remove_cvref_t<M>>::is_row_major)
		out << "^T";

	return out;
}

template <LazyVectorLike V>
std::ostream& operator<<(std::ostream& out, V&& expr)
{
	const auto dim{ expr.size() };

	out << '(';
	for (int i{0}; i < dim - 1; i++)
		out << expr[i] << ", ";
	out << expr[dim - 1] << ')' << \
		(internal::Traits<std::remove_cvref_t<V>>::is_row_major ? "" : "^T");

	return out;
}

//----------------------------------------------------------------------------------
// Constraint templates
//----------------------------------------------------------------------------------

// Scalar multiplication
template <typename L, typename R>
	requires (LazyType<L> && LazyScalarLike<R>) ||
					 (LazyScalarLike<L> && LazyType<R>)
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

// Multiply-Assign, only for materialized types
template <LazyMaterializedType L, typename R>
	requires (LazyMaterializedType<R> || LazyScalarLike<R>) && 
					 (LazyType<L> && LazyScalarLike<R>) ||
					 (LazyScalarLike<L> && LazyType<R>) &&
					 (!(LazyScalarLike<L> && !LazyScalarLike<R>))
constexpr auto operator*=(L&& left, R&& right)
{
	return left = (left * right).eval();	
}
		
// Scalar division
template <typename L, typename R>
	requires (LazyType<L> && LazyScalarLike<R>) ||
					 (LazyScalarLike<L> && LazyType<R>)
constexpr auto operator/(L&& left, R&& right)
{
	using lwt = decltype(wrap_scalar(left));
	using rwt = decltype(wrap_scalar(right));

	// Deduce underlying type
	using T = decltype(lwt(left).evaluate(0) * rwt(right).evaluate(0));

	//TODO: SOO
	using ltraits = internal::Traits<std::remove_cvref_t<lwt>>;
	using rtraits = internal::Traits<std::remove_cvref_t<rwt>>;
	using max_traits = std::conditional_t<
		(ltraits::size > rtraits::size),
		ltraits,
		rtraits
	>;

	const auto left_w { wrap_scalar(left) };
	const auto right_w{ wrap_scalar(right) };

	if constexpr (max_traits::size <= 16)
	{
		typename max_traits::materialized_type result;	
		for (int i{0}; i < max_traits::rows; i++)
			for (int j{0}; j < max_traits::columns; j++)
				result[i, j] = left_w[i, j] / right_w[i, j];

		return result;
	} else {
		return internal::DivExpr<lwt, rwt, T>(left_w, right_w);
	}
}

// Divide-Assign, only for materialized types
template <LazyMaterializedType L, typename R>
	requires (LazyMaterializedType<R> || LazyScalarLike<R>) && 
					 (LazyType<L> && LazyScalarLike<R>) ||
					 (LazyScalarLike<L> && LazyType<R>) &&
					 (!(LazyScalarLike<L> && !LazyScalarLike<R>))
constexpr auto operator/=(L&& left, R&& right)
{
	return left = (left / right).eval();	
}

END_FCP_OPERATORS_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_H_LAZY_OPERATORS_HPP
