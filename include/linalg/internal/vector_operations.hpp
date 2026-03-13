#ifndef FCP_MATH_H_LAZY_VECTOR_OPERATIONS_HPP
#define FCP_MATH_H_LAZY_VECTOR_OPERATIONS_HPP

#include "core/common.hpp"
#include "linalg/operators.hpp"
#include "linalg/matrix.hpp"

#include <cmath>
#include <numeric>
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

/*
 * TODO:
 * - mask operations
 */

// Dot product (vector-vector)
template <LazyVectorLike L, LazyVectorLike R>
constexpr auto cdot(L&& left, R&& right)
{
	return std::inner_product(left.begin(), left.end(), right.begin(), 0);	
}

// Calculate norm of a vector
template <LazyVectorLike Expr>
constexpr auto norm(Expr&& expr)
{
	return cdot(expr, expr);
}

// Normalize vector
template <LazyVectorLike E>
constexpr auto normalize(E vec)
{
	using operators::operator/;
	using traits = internal::Traits<std::remove_cvref_t<E>>;
	using T = traits::element_type;

	// Deduce vector type from expression
	using vec_t = std::conditional_t<
		traits::is_row_major,
		RowVector<T, traits::columns>,
		ColumnVector<T, traits::rows>
	>;

	vec_t result = (vec / norm(vec));
	return result;
}

START_FCP_INTERNAL_NAMESPACE
template <typename LeftExpr, typename RightExpr, typename T>
struct CrossProductExpr;

template <typename LeftExpr, typename RightExpr, typename T>
struct Traits<CrossProductExpr<LeftExpr, RightExpr, T>>
{
	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = Traits<std::remove_cvref_t<RightExpr>>;
	using merger  = MatrixFlagsMerger<ltraits::flags, rtraits::flags>;

	using element_type = T;

	static constexpr int rows{ ltraits::rows };
	static constexpr int columns{ ltraits::columns };
	static constexpr int size{ 3 };

	static constexpr int flags{ merger::value };

	using materialized_type = Matrix<T, rows, columns, flags>;

	static constexpr bool is_row_major{ ltraits::is_row_major };
	static constexpr bool is_writable{ false };
};

template <typename LeftExpr, typename RightExpr, typename T>
struct CrossProductExpr : BinaryExpressionBase<LeftExpr, RightExpr, T, CrossProductExpr<LeftExpr, RightExpr, T>>
{
	using base = BinaryExpressionBase<LeftExpr, RightExpr, T, CrossProductExpr<LeftExpr, RightExpr, T>>;
	using base::base;
	using base::m_left_expr;
	using base::m_right_expr;

	constexpr T evaluate(int i) const
	{
		switch(i)
		{
			case 0:
				return m_left_expr.evaluate(1)*m_right_expr.evaluate(2) - m_right_expr.evaluate(1)*m_left_expr.evaluate(2);	
			case 1:
				return m_right_expr.evaluate(0)*m_left_expr.evaluate(2) - m_left_expr.evaluate(0)*m_right_expr.evaluate(2);
			default:
				return m_left_expr.evaluate(0)*m_right_expr.evaluate(1) - m_right_expr.evaluate(0)*m_left_expr.evaluate(1);	
		}
	}
};
END_FCP_INTERNAL_NAMESPACE

// Cross product (TODO: only 3D for now)
template <LazyVectorLike LeftExpr, LazyVectorLike RightExpr>
constexpr auto cross(LeftExpr&& left, RightExpr&& right)
{
	using ltraits = internal::Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = internal::Traits<std::remove_cvref_t<RightExpr>>;

	using element_t = std::common_type_t<typename ltraits::element_type, typename rtraits::element_type>;

	static_assert(ltraits::size == 3 && rtraits::size == 3, 
			"Cross product is only defined for 3D vectors."
	);

	return internal::CrossProductExpr<LeftExpr, RightExpr, element_t>(
		std::forward<LeftExpr>(left),
		std::forward<RightExpr>(right)
	);
}

// Calculate the magnitude of a vector
template <LazyVectorLike Expr>
constexpr auto l2norm(Expr&& expr)
{
	//TODO: sqrt is not constexpr until C++26!
	return std::sqrt(norm(expr));
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_H_LAZY_VECTOR_OPERATIONS_HPP
