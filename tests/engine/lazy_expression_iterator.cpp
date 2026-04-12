/*
 * Test expression iterator features.
 */

#include "linalg/internal/expression_templates.hpp"	// SumExpr
#include "linalg/scalar.hpp"

#include <iostream>
#include <numeric>

// Iterators have to be used differently in a compile-time context vs
// a runtime one. Here, I cannot use a range-based loop directly, I must
// use a constexpr function and ensure that its result is a constant expression.
// Alternatively, I can use a constexpr lambda. In general, the solution lies in
// ensuring that the (range-based) loop is in a constexpr context.
// In a runtime context I can use range-based loops.

int main()
{
	//------------------------------------------------------------
	// Compile-time iterator interface
	//------------------------------------------------------------
	using fcp::math::internal::SumExpr;
	using Float = fcp::math::Scalar<float>;

	constexpr auto sum = SumExpr<Float, Float, float>(3.5, 4.5);

	constexpr auto loop_ct{
		[&]() {
			float res = 0.f;
			for (auto s : sum)
				res += s;
			return res;
		}()
	};

	constexpr float acc_ct = std::accumulate(sum.begin(), sum.end(), 0.f);
	static_assert(acc_ct == 8.f);

	//------------------------------------------------------------
	// Compile-time iterator interface
	//------------------------------------------------------------
	for (auto s : sum)
		std::cout << s << '\n';

}
