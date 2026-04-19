#ifndef FCP_MATH_LINALG_SCALAR_HPP
#define FCP_MATH_LINALG_SCALAR_HPP

#include "core/internal/common.hpp"
#include "core/internal/interface_base.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Scalar wrapper class template
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE
template <typename T>
struct Traits<Scalar<T>>
{
	using element_type = T;
	using materialized_type = T;

	static constexpr int rows{ 1 };
	static constexpr int columns{ 1 };
	static constexpr int size{ 1 };
	static constexpr int flags{ JollyFlag };	// Compatible with all options

	static constexpr bool is_row_major{ true };
	static constexpr bool is_writable{ true };
};

//template <typename T>
//	requires std::is_scalar_v<T>
//struct Traits<T>
//{
//	using element_type = T;
//	using materialized_type = Scalar<T>;
//	using mtraits = Traits<materialized_type>;
//
//	static constexpr int rows{ mtraits::rows };
//	static constexpr int columns{ mtraits::columns };
//	static constexpr int size{ mtraits::size };
//	static constexpr int flags{ mtraits::flags };
//
//	static constexpr bool is_row_major{ mtraits::is_row_major };
//	static constexpr bool is_writable{ mtraits::is_writable };
//};
END_FCP_INTERNAL_NAMESPACE

template <typename T>
struct Scalar : internal::InterfaceBase<Scalar<T>>
{
	using is_scalar = void;

	FCPM_ALWAYS_INLINE
	constexpr void swap_rows_impl(int, int) const {}

	FCPM_ALWAYS_INLINE
	constexpr void swap_cols_impl(int, int) const {}

	constexpr Scalar() = default;
	constexpr Scalar(const T value): m_value{value} {}
	constexpr Scalar(const Scalar& other): m_value{other.m_value} {}
	constexpr Scalar& operator=(const Scalar& other){ m_value = other.m_value; return *this; }
	
	// Lazy constructor
	constexpr Scalar(LazyExpressionType auto expr): m_value{expr.evaluate(0)} {}
	// Lazy assignment
	constexpr Scalar operator=(LazyExpressionType auto expr) { m_value = expr.evaluate(0); }

	FCPM_ALWAYS_INLINE constexpr T evaluate(int) const { return m_value; }
	FCPM_ALWAYS_INLINE constexpr T evaluate(int, int) const { return m_value; }

	const T m_value;
};

// Helper function template that wraps a scalar value
template <typename T>
FCPM_ALWAYS_INLINE FCPM_CONST FCPM_FLATTEN
constexpr decltype(auto) wrap_scalar(T&& value)
{
	using no_cvref_t = std::remove_cvref_t<T>;

	if constexpr (std::is_arithmetic_v<no_cvref_t>)
#ifdef FCPM_USE_STD_FORWARD
		return Scalar<no_cvref_t>{std::forward<T>(value)};
#else
	return Scalar<no_cvref_t>{static_cast<T&&>(value)};
#endif
	else
#ifdef FCPM_USE_STD_FORWARD
		return std::forward<T>(value);
#else
		return static_cast<T&&>(value);
#endif
}

//TODO: this in combination with static_cast() induces the compiler
//			call the copy constructor of the class T, incurring in 
//			undesired overhead.
//			However, wrap_scalar() induces the compiler not to inline the
//			function call. The solution is to probably use a force-inline
//			directive
//template <typename T>
//struct wrap_scalar_trait
//{
//	using no_cvref_t = std::remove_cvref_t<T>;
//
//	using type = std::conditional_t<
//		std::is_arithmetic_v<no_cvref_t>,
//		Scalar<no_cvref_t>,
//		T
//	>;
//};
//
//template <typename T>
//using wrap_scalar_t = wrap_scalar_trait<T>::type;

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_SCALAR_HPP
