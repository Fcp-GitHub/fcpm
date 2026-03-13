#ifndef FCP_MATH_CORE_BASE_HPP
#define FCP_MATH_CORE_BASE_HPP

#define START_FCP_NAMESPACE namespace fcp {
#define END_FCP_NAMESPACE }

#define START_FCP_MATH_NAMESPACE namespace math {
#define END_FCP_MATH_NAMESPACE }

#define START_FCP_INTERNAL_NAMESPACE namespace internal {
#define END_FCP_INTERNAL_NAMESPACE }

#define START_FCP_OPERATORS_NAMESPACE namespace operators {
#define END_FCP_OPERATORS_NAMESPACE }

#include <utility>
#include <type_traits>
#include <concepts>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

// Compile-time switch statement
template <typename T, typename... Choices>
struct match_type : std::false_type{};	// Base case

template <typename T, typename Choice, typename Result, typename... Others>
struct match_type<T, Choice, Result, Others...>
{
	using type = std::conditional_t<
		std::is_same_v<T, Choice>,	// if T matches Choice...
		Result,										 	// use Result 
		typename match_type<T, Others...>::type // else go to the next recursive step
	>;	
};

template <typename T, typename... Choices>
using match_type_t = match_type<T, Choices...>::type;

// Compile-time for loop
template <int From, int To>
constexpr void for_constexpr(auto&& body)
{
	if constexpr (From < To)
	{
		// Pass the index to the loop body
		body(std::integral_constant<int, From>{});

		// Recursively go to the next index
		for_constexpr<From+1, To>(std::forward<decltype(body)>(body));
	}
}

//----------------------------------------------------------------------------------
// Concepts
//----------------------------------------------------------------------------------

// Accept only operands that support lazy evaluation
template <typename E>
concept LazyType = requires
{
	typename std::remove_cvref_t<E>::is_lazy;
};

// Expression concept
template <typename E>
concept LazyExpressionType = LazyType<E> && requires(E expr)
{
	typename std::remove_cvref_t<E>::is_expression;
};

// Matrix concept
template <typename E>
concept LazyMatrixType = LazyType<E> && requires(E expr)
{
	typename std::remove_cvref_t<E>::is_matrix;
	(expr.rows() > 1) and (expr.columns() > 1);
};

// Matrix or expression resulting in one
template <typename E>
concept LazyMatrixLike = (
		LazyExpressionType<E> &&
	  requires(E expr) { (expr.rows() > 1) and (expr.columns() > 1); }
	) || 
	LazyMatrixType<E>; 

// Vector concept
template <typename E>
concept LazyVectorType = LazyType<E> && requires(E expr)
{
	typename std::remove_cvref_t<E>::is_matrix;
	(expr.rows() == 1) or (expr.columns() == 1);
};

// Vector or expression resulting in one
template <typename E>
concept LazyVectorLike = (
		LazyExpressionType<E> &&
		requires(E expr) { (expr.rows() == 1) or (expr.columns() == 1); }
	) ||
  LazyVectorType<E>;

// Scalar concept
//template <typename E>
//concept ScalarType = std::is_scalar_v<std::remove_cvref_t<E>> || requires
//{
//	typename std::remove_cvref_t<E>::is_scalar;
//};

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
		requires(E expr) { (expr.rows() == 1) and (expr.columns() == 1); }
	) ||
	ScalarType<E>;

START_FCP_INTERNAL_NAMESPACE

//----------------------------------------------------------------------------------
// Traits
//----------------------------------------------------------------------------------

// Detect whether a given type is a lazy matrix
template <typename T, typename = void>
struct is_lazy_matrix : std::false_type {};

template <typename T>
struct is_lazy_matrix<T, std::void_t<typename T::is_matrix>> : std::true_type {};

template <typename T>
inline constexpr bool is_lazy_matrix_v = is_lazy_matrix<std::remove_cvref_t<T>>::value;

//----------------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------------

// Type alias used to optimize data storage based on the type of the expression
template <typename E>
struct subexpr
{
	using no_cvref_t = std::remove_cvref_t<E>;
	
	using type = std::conditional_t<
		is_lazy_matrix_v<E>,
		const no_cvref_t&,
		const no_cvref_t
	>;;
};

template <typename E>
using subexpr_t = subexpr<E>::type;

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_BASE_HPP
