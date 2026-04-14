#ifndef FCPUT_LAZYEVAL_LAZY_ENGINE_HPP
#define FCPUT_LAZYEVAL_LAZY_ENGINE_HPP

#include "core/common.hpp"
#include "core/interface_base.hpp"

#include <cstddef>
#include <optional>
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

//----------------------------------------------------------------------------------
// The engine
//----------------------------------------------------------------------------------

//template <typename Expr, typename T, typename Derived>
//struct Traits<UnaryExpressionBase<Expr, T, Derived>>
//{
//	using etraits = Traits<std::remove_cvref_t<Expr>>;
//	using element_type = T;
//	
//	static constexpr int rows{ etraits::rows };
//	static constexpr int columns{ etraits::columns };
//	static constexpr int size{ rows * columns };
//
//	static constexpr bool is_row_major{ etraits::is_row_major };
//	static constexpr bool is_writable{ false };
//};

// Unary expression template base
template <typename Expr, typename T, typename Derived>
struct UnaryExpressionBase : InterfaceBase<Derived>
{
	using is_expression = void;

	// Data member
	subexpr_t<Expr> m_expr;	// This holds the result of the sub-expression
	
	// Constructor
	// Values passed as references (because of the is_heavy attribute)
	// will be stored as references, others by value
	constexpr UnaryExpressionBase(subexpr_t<Expr> e):
		m_expr{e} {}

	constexpr UnaryExpressionBase():
		m_expr{} {}
};

template <typename Expr, typename T, typename UnaryOp>
struct Traits<ElementwiseUnaryExpression<Expr, T, UnaryOp>>
{
	using etraits = Traits<std::remove_cvref_t<Expr>>;
	using element_type = T;
	using materialized_type = etraits::materialized_type;

	static constexpr int rows{ etraits::rows };
	static constexpr int columns{ etraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ etraits::flags };

	static constexpr bool is_row_major{ etraits::is_row_major };
	static constexpr bool is_writable{ false };
};

template <typename Expr, typename T, typename UnaryOp>
struct ElementwiseUnaryExpression : 
	UnaryExpressionBase<
		Expr, T, 
		ElementwiseUnaryExpression<Expr, T, UnaryOp>
	>
{
	using UnaryExpressionBase<Expr, T, ElementwiseUnaryExpression<Expr, T, UnaryOp>>::UnaryExpressionBase;
	using UnaryExpressionBase<Expr, T, ElementwiseUnaryExpression<Expr, T, UnaryOp>>::m_expr;

	constexpr T evaluate(int i) const
	{
		return UnaryOp()(m_expr.evaluate(i));
	}

	constexpr T evaluate(int row, int col) const
	{
		return UnaryOp()(m_expr.evaluate(row, col));
	}
};

template <typename Expr, typename T, typename UnaryOp>
struct Traits<MemoizedUnaryExpression<Expr, T, UnaryOp>>
{
	using etraits = Traits<std::remove_cvref_t<Expr>>;
	using element_type = T;
	using result_type  = etraits::result_type;	// TODO

	static constexpr int rows{ etraits::rows };
	static constexpr int columns{ etraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ etraits::flags };

	static constexpr bool is_row_major{ etraits::is_row_major };
	static constexpr bool is_writable{ false };
};

template <typename Expr, typename T, typename UnaryOp>
struct MemoizedUnaryExpression : 
	UnaryExpressionBase<
		Expr, T,
		MemoizedUnaryExpression<Expr, T, UnaryOp>
	>
{
	using UnaryExpressionBase<Expr, T, MemoizedUnaryExpression<Expr, T, UnaryOp>>::UnaryExpressionBase;
	using UnaryExpressionBase<Expr, T, MemoizedUnaryExpression<Expr, T, UnaryOp>>::rows;
	using UnaryExpressionBase<Expr, T, MemoizedUnaryExpression<Expr, T, UnaryOp>>::columns;
	using UnaryExpressionBase<Expr, T, MemoizedUnaryExpression<Expr, T, UnaryOp>>::flags;
	using UnaryExpressionBase<Expr, T, MemoizedUnaryExpression<Expr, T, UnaryOp>>::m_expr;

	mutable std::optional<Matrix<T, rows(), columns(), flags()>> m_buffer;

	constexpr auto evaluate(std::size_t row, std::size_t col) const
	{
		if (not m_buffer)
			materialize();
		return (*m_buffer)[row, col];
	}

	constexpr void materialize() const
	{
		// Trigger expression evaluation and store the result
		Matrix<T, rows(), columns(), flags()> temp = m_expr;

		// Apply specific algorithm to it
		(*m_buffer) = UnaryOp()(temp);	
	}
};

//template <typename LeftExpr, typename RightExpr, typename T, typename Derived>
//struct Traits<BinaryExpressionBase<LeftExpr, RightExpr, T, Derived>>
//{
//	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
//	using element_type = T;
//
//	// Assumes that both expressions have the same dimensions
//	static constexpr int rows{ ltraits::rows };
//	static constexpr int columns{ ltraits::columns };
//	static constexpr int size{ rows * columns };
//
//	// Assumes that both expressions have the same storage method
//	static constexpr bool is_row_major{ ltraits::is_row_major };
//	static constexpr bool is_writable{ false };
//};

// Binary expression template base
template <typename LeftExpr, typename RightExpr, typename T, typename Derived>
struct BinaryExpressionBase : InterfaceBase<Derived>
{
	using is_expression = void;

	// Data members
	subexpr_t<LeftExpr> m_left_expr;	// This will hold the result of the lext sub-expression
	subexpr_t<RightExpr> m_right_expr;	// This will hold the result of the right sub-expression

	// Constructor
	// Values passed as references (because of the is_heavy attribute)
	// will be stored as references, others by value
	constexpr BinaryExpressionBase(subexpr_t<LeftExpr> l, subexpr_t<RightExpr> r):
 		m_left_expr{l}, m_right_expr{r} {}

	constexpr BinaryExpressionBase():
		m_left_expr{}, m_right_expr{} {}
};

template <typename LeftExpr, typename RightExpr, typename T, typename BinaryOp>
struct Traits<ElementwiseBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>
{
	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = Traits<std::remove_cvref_t<RightExpr>>;

	// Save traits of "biggest" object
	// for broadcast operations
	using max_traits = std::conditional_t<
		(ltraits::size > rtraits::size),
		ltraits,
		rtraits
	>;

	using merger  = MatrixFlagsMerger<ltraits::flags, rtraits::flags>;
	using element_type = T;

	using materialized_type = typename max_traits::materialized_type;
	
	// Assumes that both expressions have the same dimensions
	static constexpr int rows{ max_traits::rows };
	static constexpr int columns{ max_traits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ merger::value };

	// Assumes that both expressions have the same storage method
	static constexpr bool is_row_major{ merger::use_row_major };
	static constexpr bool is_writable{ false };
};

template <typename LeftExpr, typename RightExpr, typename T, typename BinaryOp>
struct ElementwiseBinaryExpression : 
	BinaryExpressionBase<
		LeftExpr, RightExpr, T, 
		ElementwiseBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>
	>
{
	using BinaryExpressionBase<LeftExpr, RightExpr, T, ElementwiseBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::BinaryExpressionBase;
	using BinaryExpressionBase<LeftExpr, RightExpr, T, ElementwiseBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::m_left_expr;
	using BinaryExpressionBase<LeftExpr, RightExpr, T, ElementwiseBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::m_right_expr;

	// This is the core of the lazy mechanism.
	// When the Vector class calls operator[](), the compiler will
	// recursively instantiate class templates of type L_EXPR
	// and R_EXPR (type T or an expression template type) in
	// a way that can be visualized using a binary tree.
	//
	// The cost at compile time is proportional to the number of
	// expressions saved before calling a trigger operation. 
	// Template instantiation will recurse until a it finds the 
	// base case (`operator[]()` defined in the LazyVector class).
	//
	// The BinaryOp functor should generalize the process of defining the 
	// right operation for each expression template.
	constexpr T evaluate(int i) const
	{
		return BinaryOp()(m_left_expr.evaluate(i), m_right_expr.evaluate(i));
	}

	constexpr T evaluate(int row, int col) const 
	{
		return BinaryOp()(m_left_expr.evaluate(row, col), m_right_expr.evaluate(row, col));
	}
};

template <typename LeftExpr, typename RightExpr, typename T, typename BinaryOp>
struct Traits<MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>
{
	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = Traits<std::remove_cvref_t<RightExpr>>;
	using merger  = MatrixFlagsMerger<ltraits::flags, rtraits::flags>;
	using element_type = T;
	using materialized_type = ltraits::materialized_type;	//TODO

	// Assumes that both expressions have the same dimensions
	static constexpr int rows{ ltraits::rows };
	static constexpr int columns{ ltraits::columns };
	static constexpr int size{ rows * columns };

	static constexpr int flags{ merger::value };

	// Assumes that both expressions have the same storage method
	static constexpr bool is_row_major{ merger::use_row_major };
	static constexpr bool is_writable{ false };
};

template <typename LeftExpr, typename RightExpr, typename T, typename BinaryOp>
struct MemoizedBinaryExpression : 
	BinaryExpressionBase<
		LeftExpr, RightExpr, T,
		MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>		
	>
{
	using BinaryExpressionBase<LeftExpr, RightExpr, T, MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::BinaryExpressionBase;
	using BinaryExpressionBase<LeftExpr, RightExpr, T, MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::rows;
	using BinaryExpressionBase<LeftExpr, RightExpr, T, MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::columns;
	using BinaryExpressionBase<LeftExpr, RightExpr, T, MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::flags;
	using BinaryExpressionBase<LeftExpr, RightExpr, T, MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::m_left_expr;
	using BinaryExpressionBase<LeftExpr, RightExpr, T, MemoizedBinaryExpression<LeftExpr, RightExpr, T, BinaryOp>>::m_right_expr;

	mutable std::optional<Matrix<T, rows(), columns(), flags()>> m_buffer;

	constexpr auto evaluate(std::size_t row, std::size_t col) const
	{
		if (not m_buffer)
			materialize();
		return (*m_buffer)[row, col];
	}

	constexpr void materialize() const
	{
		// Trigger expression evaluation and store the results
		Matrix<T, rows(), columns(), flags()> temp_left{ m_left_expr };
		Matrix<T, rows(), columns(), flags()> temp_right{ m_right_expr };

		// Apply specific algorithm to them
		(*m_buffer) = BinaryOp()(temp_left, temp_right);	
	}
};

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCPUT_LAZYEVAL_LAZY_ENGINE_HPP
