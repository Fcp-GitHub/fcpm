#ifndef FCP_MATH_GEOMETRY_INTERNAL_QUATERNION_OPERATIONS_HPP
#define FCP_MATH_GEOMETRY_INTERNAL_QUATERNION_OPERATIONS_HPP

#include "core/internal/common.hpp"
#include "core/math_fun.hpp"
#include "core/operators.hpp"

#include "linalg/internal/vector_operations.hpp"

#include <numeric>	// inner_product
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

//----------------------------------------------------------------------------------
// Conjugation Expression Template
//----------------------------------------------------------------------------------

// Specialization for Quaternion class

template <typename T>
struct Traits<ConjugateExpr<Quaternion<T>, T>>
{
	using qtraits = Traits<std::remove_cvref_t<Quaternion<T>>>;
	using element_type = T;
	using materialized_type = Quaternion<T>;

	static constexpr int rows{ qtraits::rows };
	static constexpr int columns{ qtraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ qtraits::flags };

	static constexpr bool is_row_major{ qtraits::is_row_major };
	static constexpr bool is_writable{ false };
};

template <typename T>
struct ConjugateExpr<Quaternion<T>, T> : 
	UnaryExpressionBase<Quaternion<T>, T, ConjugateExpr<Quaternion<T>, T>>
{
	using UnaryExpressionBase<Quaternion<T>, T, ConjugateExpr<Quaternion<T>, T>>::UnaryExpressionBase;
	using UnaryExpressionBase<Quaternion<T>, T, ConjugateExpr<Quaternion<T>, T>>::m_expr;
	using traits = Traits<std::remove_cvref_t<Quaternion<T>>>;

	constexpr T evaluate(int i) const
	{
		return (i == 0) ? m_expr.evaluate(0) : (-m_expr.evaluate(i));
	}
};

// Specialization for UnitQuaternion class

template <typename T>
struct Traits<ConjugateExpr<UnitQuaternion<T>, T>>
{
	using qtraits = Traits<std::remove_cvref_t<UnitQuaternion<T>>>;
	using element_type = T;
	using materialized_type = UnitQuaternion<T>;

	static constexpr int rows{ qtraits::rows };
	static constexpr int columns{ qtraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ qtraits::flags };

	static constexpr bool is_row_major{ qtraits::is_row_major };
	static constexpr bool is_writable{ false };
};

template <typename T>
struct ConjugateExpr<UnitQuaternion<T>, T> : 
	UnaryExpressionBase<UnitQuaternion<T>, T, ConjugateExpr<UnitQuaternion<T>, T>>
{
	using UnaryExpressionBase<UnitQuaternion<T>, T, ConjugateExpr<UnitQuaternion<T>, T>>::UnaryExpressionBase;
	using UnaryExpressionBase<UnitQuaternion<T>, T, ConjugateExpr<UnitQuaternion<T>, T>>::m_expr;
	using traits = Traits<std::remove_cvref_t<UnitQuaternion<T>>>;

	constexpr T evaluate(int i) const
	{
		return (i == 0) ? m_expr.evaluate(0) : (-m_expr.evaluate(i));
	}
};

//----------------------------------------------------------------------------------
// Hamilton Product Expression Template
//----------------------------------------------------------------------------------

template <typename LeftExpr, typename RightExpr, typename T>
struct HamiltonProdExpr;

template <typename LeftExpr,typename RightExpr, typename T>
struct Traits<HamiltonProdExpr<LeftExpr, RightExpr, T>>
{
	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = Traits<std::remove_cvref_t<RightExpr>>;
	using merger  = MatrixFlagsMerger<ltraits::flags, rtraits::flags>;

	using element_type = T;

	static constexpr int rows{ ltraits::rows };
	static constexpr int columns{ ltraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ merger::value };

	using materialized_type = Quaternion<T>;

	static constexpr bool is_row_major{ ltraits::is_row_major };
	static constexpr bool is_writable{ false };

};

template <typename LeftExpr, typename RightExpr, typename T>
struct HamiltonProdExpr: BinaryExpressionBase<LeftExpr, RightExpr, T, HamiltonProdExpr<LeftExpr, RightExpr, T>>
{
	using base = BinaryExpressionBase<LeftExpr, RightExpr, T, HamiltonProdExpr<LeftExpr, RightExpr, T>>;	
	using base::base;
	using base::m_left_expr;
	using base::m_right_expr;

	constexpr T evaluate(int i) const
	{
		auto& a{ m_left_expr };
		auto& b{ m_right_expr };

		switch(i)
		{
			case 0:  return a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
			case 1:  return a[0]*b[1] + a[1]*b[0] + a[2]*b[3] - a[3]*b[2];
			case 2:  return a[0]*b[2] - a[1]*b[3] + a[2]*b[0] + a[3]*b[1];
			default: return a[0]*b[3] + a[1]*b[2] - a[2]*b[1] + a[3]*b[0];
		}
	}
};

END_FCP_INTERNAL_NAMESPACE

START_FCP_OPERATORS_NAMESPACE

template <typename L, typename R>
	requires (LazyQuaternionLike<L> && LazyQuaternionLike<R>)
constexpr auto operator*(L&& left, R&& right)
{
	using lwt = std::remove_cvref_t<L>;
	using rwt = std::remove_cvref_t<R>;
	
	// Deduce underlying type
	using T = std::common_type_t<
		typename internal::Traits<lwt>::element_type,
		typename internal::Traits<rwt>::element_type
	>;

	return internal::HamiltonProdExpr<lwt, rwt, T>(left, right);
}

// Multiply-Assign operator, only for materialized types
template <LazyMaterializedType L, LazyMaterializedType R>
	requires (LazyQuaternionLike<L> && LazyQuaternionLike<R>)
constexpr auto operator*=(L&& left, R&& right)
{
	return left = (left * right);
}

END_FCP_OPERATORS_NAMESPACE

//----------------------------------------------------------------------------------
// Norm
//----------------------------------------------------------------------------------

// Calculate norm of a quaternion
//NOTE: it is better to materialize the quaternion before this procedure
template <LazyQuaternionLike Q>
constexpr auto l2norm(const Q& q)
{
	using qtraits = internal::Traits<std::remove_cvref_t<Q>>;
	using T = qtraits::element_type;

	return fcp::math::sqrt<T>(
		q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]		
	);
}

// Returns `||q||^2`
//NOTE: it is better to materialize the quaternion before this procedure
template <LazyQuaternionLike Q>
constexpr auto l2norm_sq(const Q& q)
{
	return q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
}

//----------------------------------------------------------------------------------
// Normalization
//----------------------------------------------------------------------------------

// Returns normalized quaternion
//NOTE: it is better to materialize the quaternion before this procedure
template <LazyQuaternionLike Q>
constexpr auto normalize(const Q& q)
{
	using operators::operator/;
	return q / l2norm(q);
}

//----------------------------------------------------------------------------------
// Dot Product
//----------------------------------------------------------------------------------

// Dot product (quaternion-quaternion)
template <LazyQuaternionLike L, LazyQuaternionLike R>
constexpr auto dot(L&& left, R&& right)
{
	return std::inner_product(left.begin(), left.end(), right.begin(), 0);
}

//----------------------------------------------------------------------------------
// Logarithm
//----------------------------------------------------------------------------------

//TODO: Logarithm of a generic quaternion

// Logarithm of a unit quaternion
template <LazyUnitQuaternionLike Q>
constexpr auto ln(const Q& q)
{
	using T = internal::Traits<std::remove_cvref_t<Q>>::element_type;

	const auto half_angle{ fcp::math::acos(q.scalar()) };
	const auto temp{ half_angle / fcp::math::sin(half_angle) };

	return std::remove_cvref_t<Q>{
		static_cast<T>(0),
		q[1] * temp,
		q[2] * temp,
		q[3] * temp
	};
}

//----------------------------------------------------------------------------------
// Exponential
//----------------------------------------------------------------------------------

// Exponential of a unit quaternion
template <LazyUnitQuaternionLike Q>
constexpr auto exp(const Q& q)
{
	const auto half_angle{ fcp::math::l2norm(q.vector()) };
	const auto temp{ fcp::math::sin(half_angle) / half_angle };

	return std::remove_cvref_t<Q>{
		fcp::math::cos(half_angle),
		q[1] * temp,
		q[2] * temp,
		q[3] * temp
	};
}

//----------------------------------------------------------------------------------
// Power
//----------------------------------------------------------------------------------

// Power of a unit quaternion
template <LazyUnitQuaternionLike Q, typename T>
constexpr auto pow(const Q& q, T n)
{
	if (fcp::math::abs(q.scalar()) < static_cast<T>(0.9999))
	{
		// Actually half angle
		T angle{ fcp::math::acos(q.scalar()) };

		T new_angle{ angle * n };

		T temp{ fcp::math::sin(new_angle) / fcp::math::sin(angle) };

		return std::remove_cvref_t<Q>{
			fcp::math::cos(new_angle),
			q[1] * temp,
			q[2] * temp,
			q[3] * temp
		};	
	} else {
		return q;
	}
}

//----------------------------------------------------------------------------------
// Compute angular displacement between two orientations
//----------------------------------------------------------------------------------

// Compute angular displacement between two orientations
//CONVENTION: displacement from left quaternion to right one
template <LazyUnitQuaternionLike L, LazyUnitQuaternionLike R>
constexpr auto quat_diff(const L& left, const R& right)
{
	using operators::operator*;
	return right * left.inverse();
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_GEOMETRY_INTERNAL_QUATERNION_OPERATIONS_HPP
