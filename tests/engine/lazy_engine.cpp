/*
 * Test engine header features.
 */

#include "linalg/internal/engine.hpp"
#include "linalg/internal/expression_templates.hpp"	// SumExpr, NegExpr
#include "linalg/scalar.hpp"

// The expression templates are used to test the correct evaluation of 
// expression templates composed of both binary and unary sub-expressions

#include <iostream>

int main()
{
	//------------------------------------------------------------
	// Compile-time programming with Scalar class and manual 
	// composition of template expressions
	//------------------------------------------------------------

	using Float = fcp::math::Scalar<float>;
	using fcp::math::internal::SumExpr, fcp::math::internal::NegExpr;

	constexpr Float a_ct{ 3.5 };
	constexpr float c_ct{ 4.5 };
	constexpr auto b_ct{ fcp::math::wrap_scalar(c_ct) };

	static_assert(a_ct.evaluate(150) == 3.5);
	static_assert(b_ct.m_value == 4.5);

	constexpr auto sum_ct = SumExpr<Float, Float, float>(a_ct, b_ct);
	static_assert(sizeof(sum_ct) == 8);
	static_assert(sum_ct.evaluate(0) == 8.f);

	constexpr auto neg_ct = NegExpr<Float, float>(b_ct);
	
	constexpr auto sumneg_ct = SumExpr<SumExpr<Float, Float, float>, NegExpr<Float, float>, float>(sum_ct, neg_ct);
	static_assert(sizeof(sumneg_ct) == 12);
	static_assert(sumneg_ct.evaluate(0) == 3.5f);

	//------------------------------------------------------------
	// Standard programming with Scalar class and manual 
	// composition of template expressions
	//------------------------------------------------------------

	Float a_rt{ 3.5 };
	float c_rt{ 4.5 };
	auto  b_rt{ fcp::math::wrap_scalar(c_rt) };
	
	std::cout << a_rt.evaluate(3500) << '\n';
	std::cout << b_rt.m_value << '\n';

	auto sum_rt = SumExpr<Float, Float, float>(a_rt, b_rt);
	auto neg_rt = NegExpr<Float, float>(b_rt);

	std::cout << sum_rt.evaluate(0) << '\n';

	
	auto sumneg_rt = SumExpr<SumExpr<Float, Float, float>, NegExpr<Float, float>, float>(sum_rt, neg_rt);

	std::cout << sumneg_rt.evaluate(0) << '\n';
}
