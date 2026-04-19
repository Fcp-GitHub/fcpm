#ifndef FCPUT_LAZYEVAL_EXPRESSION_TEMPLATES
#define FCPUT_LAZYEVAL_EXPRESSION_TEMPLATES

#include "core/internal/engine.hpp"

#include <functional>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE
	
//----------------------------------------------------------------------------------
// Expression templates definitions
//----------------------------------------------------------------------------------

// Binary expressions

template <typename LeftExpr, typename RightExpr, typename T>
using SumExpr = ElementwiseBinaryExpression<LeftExpr, RightExpr, T, std::plus<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using SubExpr = ElementwiseBinaryExpression<LeftExpr, RightExpr, T, std::minus<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using MulExpr = ElementwiseBinaryExpression<LeftExpr, RightExpr, T, std::multiplies<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using DivExpr = ElementwiseBinaryExpression<LeftExpr, RightExpr, T, std::divides<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using EqToExpr = MemoizedBinaryExpression<LeftExpr, RightExpr, T, std::equal_to<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using NotEqToExpr = MemoizedBinaryExpression<LeftExpr, RightExpr, T, std::not_equal_to<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using LessExpr = MemoizedBinaryExpression<LeftExpr, RightExpr, T, std::less<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using LessEqExpr = MemoizedBinaryExpression<LeftExpr, RightExpr, T, std::less_equal<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using GreaterExpr = MemoizedBinaryExpression<LeftExpr, RightExpr, T, std::greater<T>>;

template <typename LeftExpr, typename RightExpr, typename T>
using GreaterEqExpr = MemoizedBinaryExpression<LeftExpr, RightExpr, T, std::greater_equal<T>>;

// Unary expressions

template <typename Expr, typename T>
using IdeExpr = ElementwiseUnaryExpression<Expr, T, std::identity>;

template <typename Expr, typename T>
using NegExpr = ElementwiseUnaryExpression<Expr, T, std::negate<T>>;

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCPUT_LAZYEVAL_EXPRESSION_TEMPLATES
