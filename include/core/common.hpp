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

// Matrix or expression resulting in one
template <typename E>
concept LazyMatrixLike = (
		LazyExpressionType<E> &&
	  requires(E expr) 
		{ 
			(internal::Traits<std::remove_cvref_t<E>>::rows > 1) and 
			(internal::Traits<std::remove_cvref_t<E>>::columns > 1); 
		}
	) || 
	LazyMatrixType<E>; 

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

// Vector or expression resulting in one
template <typename E>
concept LazyVectorLike = (
		LazyExpressionType<E> &&
		requires 
		{ 
			(internal::Traits<std::remove_cvref_t<E>>::rows == 1) or 
			(internal::Traits<std::remove_cvref_t<E>>::columns == 1); 
		}
	) ||
  LazyVectorType<E>;

template <typename T>
concept ScalarType = requires(T a, T b) {
    { a + b } -> std::convertible_to<std::remove_cvref_t<T>>;
    { a * b } -> std::convertible_to<std::remove_cvref_t<T>>;
    { a - b } -> std::convertible_to<std::remove_cvref_t<T>>;
    { a / b } -> std::convertible_to<std::remove_cvref_t<T>>;
    requires std::is_copy_constructible_v<std::remove_cvref_t<T>>;
} || std::is_scalar_v<std::remove_cvref_t<T>>;

// Scalar or expression resulting in one
template <typename E>
concept ScalarLike = (
		LazyExpressionType<E> &&
		requires 
		{ 
			(internal::Traits<std::remove_cvref_t<E>>::rows == 1) and 
			(internal::Traits<std::remove_cvref_t<E>>::columns == 1); 
		}
	) ||
	ScalarType<E>;


END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_COMMON_HPP
