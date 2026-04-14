#ifndef FCP_MATH_H_LAZY_VECTOR_OPERATIONS_HPP
#define FCP_MATH_H_LAZY_VECTOR_OPERATIONS_HPP

#include "core/common.hpp"
#include "core/forward.hpp"
#include "core/math_fun.hpp"
#include "core/operators.hpp"

#include <cmath>
#include <numeric>
#include <type_traits>
#include <utility>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Dot Product
//----------------------------------------------------------------------------------

// Dot product (vector-vector)
template <LazyVectorLike L, LazyVectorLike R>
constexpr auto dot(L&& left, R&& right)
{
	using ltraits = internal::Traits<std::remove_cvref_t<L>>;	
	using rtraits = internal::Traits<std::remove_cvref_t<R>>;	
	using T = std::common_type_t<
		typename ltraits::element_type, 
		typename rtraits::element_type
	>;

	static_assert(
			ltraits::size == rtraits::size, 
			"Scalar product works with vectors of the same size."
	);

	return std::inner_product(left.begin(), left.end(), right.begin(), static_cast<T>(0));	
}


//----------------------------------------------------------------------------------
// Cross-Product (TODO: only 3D for now)
//----------------------------------------------------------------------------------

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

// Cross product
template <Lazy3DVectorLike LeftExpr, Lazy3DVectorLike RightExpr>
constexpr auto cross(LeftExpr&& left, RightExpr&& right)
{
	using ltraits = internal::Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = internal::Traits<std::remove_cvref_t<RightExpr>>;

	using element_t = std::common_type_t<typename ltraits::element_type, typename rtraits::element_type>;
	using result_t = typename internal::Traits<
		internal::CrossProductExpr<LeftExpr, RightExpr, element_t>
	>::materialized_type;

	//static_assert(ltraits::size == 3 && rtraits::size == 3, 
	//		"Cross product is only defined for 3D vectors."
	//);

	//TODO: benchmarks show that eager evaluation is better for 3D vectors
	//return internal::CrossProductExpr<LeftExpr, RightExpr, element_t>(
	//	std::forward<LeftExpr>(left),
	//	std::forward<RightExpr>(right)
	//);
	
	return result_t{
		left.evaluate(1)*right.evaluate(2) - right.evaluate(1)*left.evaluate(2),
		right.evaluate(0)*left.evaluate(2) - left.evaluate(0)*right.evaluate(2),
		left.evaluate(0)*right.evaluate(1) - right.evaluate(0)*left.evaluate(1)	
	};	
}

//----------------------------------------------------------------------------------
// Lp-Norm
//----------------------------------------------------------------------------------

template <int P, LazyVectorLike Expr>
constexpr auto lpnorm(Expr&& expr)
{
	using T = internal::Traits<std::remove_cvref_t<Expr>>::element_type;

	T sum{ static_cast<T>(0) };

	for (auto e : expr)
	{
		T temp{ fcp::math::abs<T>(e) };

		if constexpr (P == 1)
			sum += temp;
		else if constexpr (P == 2)
			sum += temp*temp;
		else
			sum += fcp::math::pow<T>(temp, P);
	}

	if constexpr (P == 1) return sum;
	if constexpr (P == 2) return fcp::math::sqrt<T>(sum);
	return fcp::math::n_root<T>(sum, P);
}

// Calculate norm of a vector
template <LazyVectorLike Expr>
constexpr auto l2norm(Expr&& expr)
{
	using T = internal::Traits<std::remove_cvref_t<Expr>>::element_type;

	return fcp::math::sqrt<T>(dot(expr, expr));
}

// Returns `||v||^2`
template <LazyVectorLike Expr>
constexpr auto l2norm_sq(Expr&& expr)
{
	return dot(std::forward<Expr>(expr), std::forward<Expr>(expr));
}

//----------------------------------------------------------------------------------
// Linfinity-Norm
//----------------------------------------------------------------------------------

template <LazyVectorLike Expr>
constexpr auto linf_norm(Expr&& expr)
{
	using T = internal::Traits<std::remove_cvref_t<Expr>>::element_type;

	T max{ static_cast<T>(0) };

	for (auto e : expr)
	{
		T temp{ fcp::math::abs<T>(e) };
		if (temp > max) max = temp;
	}

	return max;
}

//----------------------------------------------------------------------------------
// Metrics
//----------------------------------------------------------------------------------

// Generic metric function
// Pass a norm function as `NormOp` to get the result
template <auto NormOp, LazyVectorLike L, LazyVectorLike R>
constexpr auto metric(L&& left, R&& right)
{
	using operators::operator-;
	return NormOp(std::forward<L>(left) - std::forward<R>(right));
}

// Returns the distance between `left` and `right`
template <LazyVectorLike L, LazyVectorLike R>
constexpr auto dist(L&& left, R&& right)
{
	return metric<l2norm>(std::forward<L>(left), std::forward<R>(right));
}

// Returns `||left - right||^2`
template <LazyVectorLike L, LazyVectorLike R>
constexpr auto dist_sq(L&& left, R&& right)
{
	using operators::operator-;
	auto diff{ left - right };
	return dot(diff, diff); 
}

//----------------------------------------------------------------------------------
// Normalization
//----------------------------------------------------------------------------------

// Returns the normalized vector
template <LazyVectorLike E>
constexpr auto normalize(E&& vec)
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

	return (vec / l2norm(vec));
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_H_LAZY_VECTOR_OPERATIONS_HPP
