#ifndef FCP_MATH_CORE_TRAITS_HPP
#define FCP_MATH_CORE_TRAITS_HPP

#include "core/internal/base.hpp"
#include "core/internal/forward.hpp"

#include <type_traits>

//----------------------------------------------------------------------------------
// Internal traits
//----------------------------------------------------------------------------------

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

//----------------------------------------------------------------------------------
// Generic "lazy object" traits
//----------------------------------------------------------------------------------

// Detect if a given type is writable
template <typename T>
inline constexpr bool is_writable_v{
			Traits<std::remove_cvref_t<T>>::is_writable &&
			/*std::is_lvalue_reference_v<T> &&*/
			(!std::is_const_v<std::remove_reference_t<T>>)	
};

// Detect a permutation expression
template <typename T>
struct is_permutation_expr : std::false_type{};

template <typename SubExpr>
struct is_permutation_expr<PermutationExpr<SubExpr>> : std::true_type{};

template <typename Expr>
constexpr bool is_permutation_expr_v{ is_permutation_expr<std::remove_cvref_t<Expr>>::value };

// Get trait of bigger object
template <typename E1, typename E2>
struct get_max_traits
{
	using no_cvref1_t = std::remove_cvref_t<E1>;
	using no_cvref2_t = std::remove_cvref_t<E2>;
	using traits1 = internal::Traits<no_cvref1_t>;
	using traits2 = internal::Traits<no_cvref2_t>;

	using type = std::conditional_t<
		(traits1::size > traits2::size),
		traits1,
		traits2
	>;
};

// Get trait of bigger object
template <typename E1, typename E2>
using get_max_traits_t = get_max_traits<E1, E2>::type;



//----------------------------------------------------------------------------------
// Matrix traits
//----------------------------------------------------------------------------------

// Detect whether a given type is a lazy matrix
template <typename T, typename = void>
struct is_lazy_matrix : std::false_type {};

template <typename T>
struct is_lazy_matrix<T, std::void_t<typename T::is_matrix>> : std::true_type {};

template <typename T>
inline constexpr bool is_lazy_matrix_v = is_lazy_matrix<std::remove_cvref_t<T>>::value;

// Find optimal type for data storage inside an expression node
template <typename E>
struct subexpr
{
	using no_cvref_t = std::remove_cvref_t<E>;
	
	using type = std::conditional_t<
		is_lazy_matrix_v<E>,
		const no_cvref_t&,
		const no_cvref_t
	>;
};

// Type alias used to optimize data storage based on the type of the expression
template <typename E>
using subexpr_t = subexpr<E>::type;


END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_TRAITS_HPP
