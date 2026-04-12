#ifndef FCP_MATH_LINALG_MATRIX_OPS_HPP
#define FCP_MATH_LINALG_MATRIX_OPS_HPP

#include "core/base.hpp"
#include "core/common.hpp"

#include "linalg/internal/kernels/gemm.hpp"
#include "linalg/internal/kernels/mat_inverse.hpp"

#include <numeric>
#include <type_traits>
#include <optional>
#include <array>
#include <utility>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

//----------------------------------------------------------------------------------
// Permutation expression template
//----------------------------------------------------------------------------------

template <typename Expr>
struct Traits<PermutationExpr<Expr>>
{
	using etraits = Traits<std::remove_cvref_t<Expr>>;
	using element_type = etraits::element_type;

	static constexpr int rows{ etraits::columns };
	static constexpr int columns{ etraits::rows };
	static constexpr int size{ etraits::size };

	static constexpr int flags{ etraits::flags };

	using materialized_type = etraits::materialized_type;

	static constexpr bool is_row_major{ etraits::is_row_major };
	static constexpr bool is_writable{ false };
};

template <typename Expr>
struct PermutationExpr : 
	UnaryExpressionBase<
		Expr, 
		typename Traits<std::remove_cvref_t<Expr>>::element_type,
		PermutationExpr<Expr>
	>
{
	using etraits = Traits<std::remove_cvref_t<Expr>>;
	using T = etraits::element_type;
	using base = UnaryExpressionBase<Expr, T, PermutationExpr<Expr>>;

	using base::base;
	using base::m_expr;

	std::array<int, etraits::rows> m_row_indices;
	std::array<int, etraits::columns> m_col_indices;

	constexpr PermutationExpr(Expr&& expr, int row_i, int row_j, int col_i, int col_j):
		base(std::forward<Expr>(expr))
	{
		std::iota(m_row_indices.begin(), m_row_indices.end(), 0);
		std::iota(m_col_indices.begin(), m_col_indices.end(), 0);

		m_row_indices[row_i] = row_j;
		m_row_indices[row_j] = row_i;

		m_col_indices[col_i] = col_j;
		m_col_indices[col_j] = col_i;
	}

	static constexpr auto add_row_swap(Expr&& expr, int i, int j)
	{
		using raw_t = std::remove_cvref_t<Expr>;

		if constexpr (is_permutation_expr_v<raw_t>)
		{
			expr.m_row_indices[i] = j;
			expr.m_row_indices[j] = i;
			return expr;	
		} else {
			return PermutationExpr(std::forward<Expr>(expr), i, j, 0, 0);	
		}
	}

	static constexpr auto add_col_swap(Expr&& expr, int i, int j)
	{
		using raw_t = std::remove_cvref_t<Expr>;

		if constexpr (is_permutation_expr_v<raw_t>)
		{
			expr.m_col_indices[i] = j;
			expr.m_col_indices[j] = i;
			return expr;	
		} else {
			return PermutationExpr(std::forward<Expr>(expr), 0, 0, i, j);	
		}
	}

	constexpr T evaluate(int i) const
	{
		constexpr int rows{ etraits::rows    };
		constexpr int cols{ etraits::columns };

		const int row{ etraits::is_row_major ? (i/cols) : (i%rows) };
		const int col{ etraits::is_row_major ? (i%cols) : (i/rows) };

		return m_expr.evaluate(m_row_indices[row], m_col_indices[col]);
	}

	constexpr T evaluate(int row, int col) const
	{
		return m_expr.evaluate(m_row_indices[row], m_col_indices[col]);
	}
};

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

	//TODO: switch layout in traits?
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

template <typename Expr, typename T>
struct Traits<InverseExpr<Expr, T>>
{
	using etraits = Traits<std::remove_cvref_t<Expr>>;
	using element_type = T;

	static constexpr int rows{ etraits::rows };
	static constexpr int columns{ etraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ etraits::flags };

	using materialized_type = Matrix<T, rows, columns, flags>;

	static constexpr bool is_row_major{ etraits::is_row_major };
	static constexpr bool is_writable{ false };
};

template <typename Expr, typename T>
struct InverseExpr : UnaryExpressionBase<Expr, T, InverseExpr<Expr, T>>
{
	using base = UnaryExpressionBase<Expr, T, InverseExpr<Expr, T>>;
	using base::base;
	using base::m_expr;

	using etraits = Traits<std::remove_cvref_t<Expr>>;	
	using buffer_t = Matrix<T, etraits::rows, etraits::columns, etraits::flags>;
	using btraits  = Traits<buffer_t>;

	mutable std::optional<buffer_t> m_mem;

	constexpr auto evaluate(int i) const
	{
		constexpr int brows{ btraits::rows };
		constexpr int bcols{ btraits::columns };

		const int row{ btraits::is_row_major ? (i/bcols) : (i%brows) };
		const int col{ btraits::is_row_major ? (i%bcols) : (i/brows) };

		//NOTE: Do whatever is in your power to avoid this consteval branch.
		//			For most of the situations it is not a good path, even for 
		//			small matrices. The best options to compute an inverse
		//			(both at compile-time and at runtime) are:
		//			- materialize matrix through `eval()`
		//			- use the LU solver, which is fully constexpr-capable
		if consteval {
			buffer_t temp;

			kernels::inverse_dispatcher(temp, m_expr.eval());

			return temp[row, col];
		} else {

			if (!m_mem) this->materialize();

			return (*m_mem)[row, col];
		}
	}

	constexpr auto evaluate(int row, int col) const
	{
		//NOTE: Do whatever is in your power to avoid this consteval branch.
		//			For most of the situations it is not a good path, even for 
		//			small matrices. The best options to compute an inverse
		//			(both at compile-time and at runtime) are:
		//			- materialize matrix through `eval()`
		//			- use the LU solver, which is fully constexpr-capable
		if consteval {
			buffer_t temp;

			kernels::inverse_dispatcher(temp, m_expr.eval());

			return temp[row, col];
		} else {

			if (!m_mem) this->materialize();

			return (*m_mem)[row, col];
		}
	}

	constexpr void materialize() const
	{
		buffer_t mat{ mat_if_needed(m_expr) };

		m_mem.emplace(static_cast<T>(0));

		kernels::inverse_dispatcher(*m_mem, mat);	
	}

	constexpr void materialize_to(auto& buffer) const
	{
		buffer_t mat{ mat_if_needed(m_expr) };

		kernels::inverse_dispatcher(buffer, mat);
	}

	private:
		template <typename E>
		constexpr auto mat_if_needed(E&& expr) const
		{
			using raw_t = std::remove_cvref_t<E>;

			if constexpr (is_lazy_matrix_v<raw_t>)
				return std::forward<E>(expr);
			else
				return expr.eval();
		}
};

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

	static constexpr bool is_row_major{ 
		requires{ requires (LazyVectorLike<LeftExpr>); } ? 
			ltraits::is_row_major : 
			(
				requires{ requires (LazyVectorLike<RightExpr>); } ?
			 		rtraits::is_row_major :
			 		merger::use_row_major
			) 
	};

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
	using btraits = Traits<buffer_t>;
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

			return kernels::unrolled_dot<0, common>(left, right, row, col);
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

			return kernels::unrolled_dot<0, common>(left, right, row, col);
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
		//std::fill((*m_mem).begin(), (*m_mem).end(), static_cast<T>(0));

		m_mem.emplace(static_cast<T>(0)); // mandatory or std::optional doesn't see the initialization

		// Let the dispatcher choose the best routine
		kernels::gemm_dispatcher<merger::value>(*m_mem, left, right);	
	}

	constexpr void materialize_to(auto& buffer) const
	{
		lmat_t left{ mat_if_needed(m_left_expr) };
		rmat_t right{ mat_if_needed(m_right_expr) };
		//std::fill((*m_mem).begin(), (*m_mem).end(), static_cast<T>(0));

		kernels::gemm_dispatcher<merger::value>(buffer, left, right);
	}

	private:
		template <typename Expr>
		constexpr auto mat_if_needed(Expr&& expr) const
		{
			using raw_t = std::remove_cvref_t<Expr>;

			if constexpr (is_lazy_matrix_v<raw_t>)
				return std::forward<Expr>(expr);
			else
				return expr.eval();
		}
};
END_FCP_INTERNAL_NAMESPACE

START_FCP_OPERATORS_NAMESPACE
template <typename L, typename R>
	requires (
			(LazyMatrixLike<L> && LazyVectorLike<R>) || 
			(LazyVectorLike<L> && LazyMatrixLike<R>) ||
			(LazyMatrixLike<L> && LazyMatrixLike<R>)
	)
constexpr auto operator*(L&& left, R&& right)
{
	//TODO
	using lwt = std::remove_cvref_t<L>;
	using rwt = std::remove_cvref_t<R>;

	static_assert(
			internal::Traits<lwt>::columns == internal::Traits<rwt>::rows,
			"The number of left matrix's columns should be equal to the number of \
			right matrix's rows."
	);

	// Deduce underlying type
	using T = std::common_type_t<
		typename internal::Traits<lwt>::element_type,
		typename internal::Traits<rwt>::element_type
	>;

	return internal::GemmExpr<lwt, rwt, T>(left, right);
}

// Multiply-Assign operator, only for materialized types
template <LazyMaterializedType L, LazyMaterializedType R>
	requires (
			(LazyMatrixLike<L> || LazyVectorLike<L>) && 
			(LazyMatrixLike<R> || LazyVectorLike<R>) &&
			!(LazyVectorLike<L> && LazyVectorLike<R>) &&
			!(LazyScalarLike<L> && !LazyScalarLike<R>)
	)
constexpr auto operator*=(L&& left, R&& right)
{
	return left = (left * right);
}
END_FCP_OPERATORS_NAMESPACE

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_MATRIX_OPS_HPP
