#ifndef FCP_MATH_CORE_COMMON_HPP
#define FCP_MATH_CORE_COMMON_HPP

#include "core/base.hpp"
#include "core/forward.hpp"

#include <type_traits>
#include <concepts>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

template <typename T>
inline constexpr bool is_writable_v{
			Traits<std::remove_cvref_t<T>>::is_writable &&
			/*std::is_lvalue_reference_v<T> &&*/
			(!std::is_const_v<std::remove_reference_t<T>>)	
};

template <typename... Vecs>
concept AllSameLayout = 
	(Traits<std::remove_cvref_t<Vecs>>::is_row_major && ...) ||
	(!Traits<std::remove_cvref_t<Vecs>>::is_row_major && ...);

END_FCP_INTERNAL_NAMESPACE

inline constexpr int RowMajor{ 0x01 };
inline constexpr int ColumnMajor{ 0x02 };
inline constexpr int StaticStorage{ 0x04 };
inline constexpr int UseSIMD{ 0x08 };
inline constexpr int JollyFlag{ 0b1111 };	// For scalar types

inline constexpr int MatrixDefaultFlags{ RowMajor | StaticStorage };

template <typename T, int N>
using RowVector = Matrix<T, 1, N, RowMajor | StaticStorage>;

template <typename T, int N>
using ColumnVector = Matrix<T, N, 1, ColumnMajor | StaticStorage>;

START_FCP_INTERNAL_NAMESPACE

template <int Flags>
struct MatrixFlagsInspector
{
	static constexpr bool check_if_set(int flag)
	{
		return (Flags & flag) != 0;
	}
	
	static constexpr bool use_row_major{ check_if_set(RowMajor) };
	static constexpr bool use_static_storage{ check_if_set(StaticStorage) };
	static constexpr bool use_simd{ check_if_set(UseSIMD) };
	
};

template <int LeftFlags, int RightFlags>
struct MatrixFlagsMerger
{
	static constexpr int layout{ (LeftFlags & RowMajor) ? RowMajor : ColumnMajor };
	static constexpr int storage{ StaticStorage };
	static constexpr int simd{ ((LeftFlags & UseSIMD) && (RightFlags & UseSIMD)) ? UseSIMD : 0 };

	static constexpr int value{ layout | storage | simd };

	static constexpr bool use_row_major{ (layout == RowMajor) ? true : false };
};

template <typename T>
struct is_permutation_expr : std::false_type{};

template <typename SubExpr>
struct is_permutation_expr<PermutationExpr<SubExpr>> : std::true_type{};

template <typename Expr>
constexpr bool is_permutation_expr_v{ is_permutation_expr<std::remove_cvref_t<Expr>>::value };

END_FCP_INTERNAL_NAMESPACE

// Matrix concept
template <typename E>
concept LazyMatrixType = LazyType<E> && requires(E expr)
{
	typename std::remove_cvref_t<E>::is_matrix;
	requires (
			(internal::Traits<std::remove_cvref_t<E>>::rows > 1) and 
			(internal::Traits<std::remove_cvref_t<E>>::columns > 1)
	);
};

// NumRowsxNumColumns matrix is required
template <int NumRows, int NumColumns, typename E>
concept LazyRxCMatrixType = LazyMatrixType<E> && requires(E expr)
{
	requires (
		(internal::Traits<std::remove_cvref_t<E>>::rows == NumRows) &&
		(internal::Traits<std::remove_cvref_t<E>>::columns == NumColumns)
	);
};

// 2x2 matrix is required
template <typename E>
concept Lazy2x2MatrixType = LazyRxCMatrixType<2, 2, E>;

// 3x3 matrix is required
template <typename E>
concept Lazy3x3MatrixType = LazyRxCMatrixType<3, 3, E>;

// 4x4 matrix is required
template <typename E>
concept Lazy4x4MatrixType = LazyRxCMatrixType<4, 4, E>;


// Matrix or expression resulting in one
template <typename E>
concept LazyMatrixLike = LazyMatrixType<typename internal::Traits<std::remove_cvref_t<E>>::materialized_type>;

// NumRowsxNumColumns matrix-like is required
template <int NumRows, int NumColumns, typename E>
concept LazyRxCMatrixLike = LazyMatrixLike<E> && requires(E expr)
{
	requires (
		(internal::Traits<std::remove_cvref_t<E>>::rows == NumRows) &&
		(internal::Traits<std::remove_cvref_t<E>>::columns == NumColumns)
	);
};

// 2x2 matrix-like is required
template <typename E>
concept Lazy2x2MatrixLike = LazyRxCMatrixLike<2, 2, E>;

// 3x3 matrix-like is required
template <typename E>
concept Lazy3x3MatrixLike = LazyRxCMatrixLike<3, 3, E>;

// 4x4 matrix-like is required
template <typename E>
concept Lazy4x4MatrixLike = LazyRxCMatrixLike<4, 4, E>;

// Vector concept
template <typename E>
concept LazyVectorType = LazyType<E> && requires(E expr)
{
	typename std::remove_cvref_t<E>::is_matrix;
	requires (
			(internal::Traits<std::remove_cvref_t<E>>::rows == 1) or 
			(internal::Traits<std::remove_cvref_t<E>>::columns == 1)
	);
};

// N-Dimensional vector is required
template <int N, typename E>
concept LazyNDVectorType = LazyVectorType<E> && requires(E expr)
{
	requires ( internal::Traits<std::remove_cvref_t<E>>::size == N );
};

// 2D vector is required
template <typename E>
concept Lazy2DVectorType = LazyNDVectorType<2, E>;

// 3D vector is required
template <typename E>
concept Lazy3DVectorType = LazyNDVectorType<3, E>;

// 4D vector is required
template <typename E>
concept Lazy4DVectorType = LazyNDVectorType<4, E>;

// Vector or expression resulting in one
template <typename E>
concept LazyVectorLike = LazyVectorType<typename internal::Traits<std::remove_cvref_t<E>>::materialized_type>; 

// N-Dimensional vector-like is required
template <int N, typename E>
concept LazyNDVectorLike = LazyVectorLike<E> && requires(E expr)
{
	requires ( internal::Traits<std::remove_cvref_t<E>>::size == N );
};

// 2D vector-like is required
template <typename E>
concept Lazy2DVectorLike = LazyNDVectorLike<2, E>;

// 3D vector-like is required
template <typename E>
concept Lazy3DVectorLike = LazyNDVectorLike<3, E>;

// 4D vector-like is required
template <typename E>
concept Lazy4DVectorLike = LazyNDVectorLike<4, E>;

template <typename T>
concept ScalarType = requires(T a, T b) {
    { a + b } -> std::convertible_to<std::remove_cvref_t<T>>;
    { a * b } -> std::convertible_to<std::remove_cvref_t<T>>;
    { a - b } -> std::convertible_to<std::remove_cvref_t<T>>;
    { a / b } -> std::convertible_to<std::remove_cvref_t<T>>;
    requires std::is_copy_constructible_v<std::remove_cvref_t<T>>;
} || std::is_scalar_v<std::remove_cvref_t<T>> ||
requires(T a)
{
	typename std::remove_cvref_t<T>::is_scalar;
};

// Scalar or expression resulting in one
template <typename E>
concept LazyScalarLike = (
		LazyExpressionType<E> &&
		requires(E expr)
		{ 
			requires (
				(internal::Traits<std::remove_cvref_t<E>>::rows == 1) and
				(internal::Traits<std::remove_cvref_t<E>>::columns == 1) 
			);
		}
	) ||
	ScalarType<E>;

// Quaternion concept
template <typename E>
concept LazyQuaternionType = LazyType<E> && requires(E expr)
{
	typename std::remove_cvref_t<E>::is_quaternion;
};

// Unit quaternion concept
template <typename E>
concept LazyUnitQuaternionType = LazyQuaternionType<E> && requires(E expr)
{
	typename std::remove_cvref_t<E>::is_unit_quaternion;
};

// Quaternion or expression resulting in one
template <typename E>
concept LazyQuaternionLike = (
		LazyExpressionType<E> && requires(E expr)
		{
			typename internal::Traits<std::remove_cvref_t<E>>::materialized_type::is_quaternion;	
		}
	) || LazyQuaternionType<E>;

// Unit quaternion or expression resulting in one
template <typename E>
concept LazyUnitQuaternionLike = (
		LazyExpressionType<E> && requires(E expr)
		{
			typename internal::Traits<std::remove_cvref_t<E>>::materialized_type::is_unit_quaternion;	
		}
	)	|| LazyUnitQuaternionType<E>;

// Generic materialized type
template <typename E>
concept LazyMaterializedType = LazyType<E> && requires(E expr)
{
	!requires{ std::remove_cvref_t<E>::is_expression; };
};

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_COMMON_HPP
