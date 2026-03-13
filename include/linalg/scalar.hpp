#ifndef FCP_MATH_LINALG_SCALAR_HPP
#define FCP_MATH_LINALG_SCALAR_HPP

#include "core/common.hpp"
#include "core/interface_base.hpp"

#include <limits>

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
	static constexpr int flags{ std::numeric_limits<int>::max() };

	static constexpr bool is_row_major{ true };
	static constexpr bool is_writable{ true };
};
END_FCP_INTERNAL_NAMESPACE

template <typename T>
struct Scalar : internal::InterfaceBase<Scalar<T>>
{
	using is_scalar = void;

	constexpr int rows_impl() const { return 1; }
	constexpr int cols_impl() const { return 1; }
	constexpr int flags_impl() const { return 0; }	// Compatible with all options

	constexpr Scalar() = default;
	constexpr Scalar(const T value): m_value{value} {}
	constexpr Scalar(const Scalar& other): m_value{other.m_value} {}
	constexpr Scalar& operator=(const Scalar& other){ m_value = other.m_value; return *this; }
	
	// Lazy constructor
	constexpr Scalar(LazyExpressionType auto expr): m_value{expr.evaluate(0)} {}
	// Lazy assignment
	constexpr Scalar operator=(LazyExpressionType auto expr) { m_value = expr.evaluate(0); }

	constexpr T evaluate(int) const { return m_value; }
	constexpr T evaluate(int, int) const { return m_value; }

	const T m_value;
};

// Helper function template that wraps a scalar value
template <typename T>
constexpr decltype(auto) wrap_scalar(T&& value)
{
	using no_cvref_t = std::remove_cvref_t<T>;

	if constexpr (std::is_arithmetic_v<no_cvref_t>)
		return Scalar<no_cvref_t>{std::forward<T>(value)};
	else
		return std::forward<T>(value);
}

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_SCALAR_HPP
