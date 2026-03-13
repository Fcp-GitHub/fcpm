#ifndef FCP_MATH_LINALG_MATRIX_OPS_HPP
#define FCP_MATH_LINALG_MATRIX_OPS_HPP

#include "core/base.hpp"
#include "core/common.hpp"

#include "linalg/matrix.hpp"
#include "linalg/internal/solvers/gemm.hpp"

#include <type_traits>
#include <optional>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

//----------------------------------------------------------------------------------
// Transposition expression template
//----------------------------------------------------------------------------------

template <typename Expr, typename T>
struct Traits<TransposeExpr<Expr, T>>
{
	using etraits = Traits<std::remove_cvref_t<Expr>>;
	using element_type = T;

	static constexpr int rows{ etraits::columns };
	static constexpr int columns{ etraits::rows };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ etraits::flags };

	using materialized_type = Matrix<T, rows, columns, flags>;

	static constexpr bool is_row_major{ etraits::is_row_major };
	static constexpr bool is_writable{ false };
};

// Here I define a specific expression template to make transposition a zero-storage operation

template <typename Expr, typename T>
struct TransposeExpr : UnaryExpressionBase<Expr, T, TransposeExpr<Expr, T>>
{
	using UnaryExpressionBase<Expr, T, TransposeExpr<Expr, T>>::UnaryExpressionBase;
	using UnaryExpressionBase<Expr, T, TransposeExpr<Expr, T>>::m_expr;
	using traits = Traits<std::remove_cvref_t<Expr>>;

	constexpr T evaluate(int i) const
	{
		//constexpr int rows{ traits::rows };
		constexpr int cols{ traits::columns };

		// Delefate the container to do the mapping
		return m_expr.evaluate(i%cols, i/cols);
	}

	constexpr T evaluate(int row, int col) const
	{
		return m_expr.evaluate(col, row);
	}
};

//----------------------------------------------------------------------------------
// Inversion expression template
//----------------------------------------------------------------------------------

//template <typename Expr, typename T>
//using InverseExpr = MemoizedUnaryExpression<Expr, T, InverseOp>;

//----------------------------------------------------------------------------------
// MM multiplication expression template
//----------------------------------------------------------------------------------
template <typename LeftExpr, typename RightExpr, typename T>
struct Traits<GemmExpr<LeftExpr, RightExpr, T>>
{
	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = Traits<std::remove_cvref_t<RightExpr>>;
	using merger  = MatrixFlagsMerger<ltraits::flags, rtraits::flags>;
	using element_type = T;

	static constexpr int rows{ ltraits::rows };
	static constexpr int columns{ rtraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ merger::value };

	using materialized_type = Matrix<element_type, rows, columns, flags>;

	static constexpr bool is_row_major{ merger::layout };
	static constexpr bool is_writable{ false };
};

template <typename LeftExpr, typename RightExpr, typename T>
struct GemmExpr : BinaryExpressionBase<
									LeftExpr, RightExpr, T, 
									GemmExpr<LeftExpr, RightExpr, T>
									>
{
	using base = BinaryExpressionBase<LeftExpr, RightExpr, T, GemmExpr<LeftExpr, RightExpr, T>>;	
	using base::base;
	using base::m_left_expr, base::m_right_expr;

	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = Traits<std::remove_cvref_t<RightExpr>>;
	using merger  = MatrixFlagsMerger<ltraits::flags, rtraits::flags>;
	using buffer_t = Matrix<T, ltraits::rows, rtraits::columns, merger::value>;
	using btraits = Traits<std::remove_cvref_t<buffer_t>>;
	using lmat_t  = Matrix<T, ltraits::rows, ltraits::columns, ltraits::flags>;
	using rmat_t  = Matrix<T, rtraits::rows, rtraits::columns, rtraits::flags>;

	mutable std::optional<buffer_t> m_mem;

	constexpr auto evaluate(int i) const
	{
		constexpr int common{ ltraits::columns };

		constexpr int brows{ btraits::rows    };
		constexpr int bcols{ btraits::columns };

		const int row{ btraits::is_row_major ? (i/bcols) : (i%brows) };
		const int col{ btraits::is_row_major ? (i%bcols) : (i/brows) };

		//TODO: add warning for big matrices
		if consteval {
			lmat_t left{ mat_if_needed(m_left_expr) };
			rmat_t right{ mat_if_needed(m_right_expr) };

			return unrolled_dot<0, common>(left, right, row, col);
		} else {

			if (!m_mem) this->materialize();

			return (*m_mem)[row, col];
		}
	}

	constexpr auto evaluate(int row, int col) const
	{
		constexpr int common{ ltraits::columns };
		constexpr int rcols{ rtraits::columns };

		//TODO: add warning for big matrices
		if consteval {
			lmat_t left{ mat_if_needed(m_left_expr) };
			rmat_t right{ mat_if_needed(m_right_expr) };

			return unrolled_dot<0, common>(left, right, row, col);
		} else {

			if (!m_mem) this->materialize();

			return (*m_mem)[row, col];
		}
	}

	constexpr void materialize() const
	{
		lmat_t left{ mat_if_needed(m_left_expr) };
		rmat_t right{ mat_if_needed(m_right_expr) };

		//TODO: if the gemm routines don't use += this is not needed
		//			Thus, if all gemm routines treat m_mem as an out 
		//			parameter, an O(n^2) operation is saved
		//std::fill(m_mem.begin(), m_mem.last(), T{0});

		// Let the dispatcher choose the best routine
		gemm_dispatcher<merger::value>(*m_mem, left, right);	
	}

	constexpr void materialize_to(auto& buffer) const
	{
		lmat_t left{ mat_if_needed(m_left_expr) };
		rmat_t right{ mat_if_needed(m_right_expr) };

		gemm_dispatcher<merger::value>(buffer, left, right);
	}

	private:
		template <typename Expr>
		constexpr auto mat_if_needed(Expr&& expr) const
		{
			using raw_t = std::remove_cvref_t<Expr>;

			if constexpr (is_lazy_matrix_v<raw_t>)
				return std::forward<Expr>(expr);
			else
				return buffer_t(expr);
		}
};
END_FCP_INTERNAL_NAMESPACE

START_FCP_OPERATORS_NAMESPACE
template <LazyMatrixLike L, LazyMatrixLike R>
constexpr auto operator*(L&& left, R&& right)
{
	//TODO
	using lwt = std::remove_cvref_t<L>;
	using rwt = std::remove_cvref_t<R>;

	// Deduce underlying type
	using T = decltype(lwt(left).evaluate(0) * rwt(right).evaluate(0));

	return internal::GemmExpr<lwt, rwt, T>(left, right);
}
END_FCP_OPERATORS_NAMESPACE

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_MATRIX_OPS_HPP
