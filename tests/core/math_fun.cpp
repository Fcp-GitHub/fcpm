#define FCPM_CMP_INTERNAL FCPM_CMP_ACCURATE
#include "core/math_fun.hpp"

#include <cassert>
#include <iostream>

#define ASSERT(x) assert((x))

int main()
{
	namespace fmi = fcp::math;

	constexpr double big{
		std::numeric_limits<double>::max() - 1.e303
	};
	std::cout << "Big double: " << big << '\n';
	std::cout << "Big double: " << big+1. << '\n';

	// Equality test
	ASSERT(fmi::cmp(2, 2));
	ASSERT(fmi::cmp(2.,2.));
	ASSERT(fmi::cmp(2.-2., 0.));
	ASSERT(!fmi::cmp(big, big+1.e303));
	ASSERT(fmi::cmp(big, big + 1.));	// Precision loss

	// Relational tests
	ASSERT(fmi::less_or_equal(2., 45.));
	ASSERT(fmi::greater_or_equal(45., 2.));

	// Absolute value
	ASSERT(fmi::cmp(fmi::abs(3.), 3.));
	ASSERT(fmi::cmp(fmi::abs(-3.), 3.));

	// Power function
	ASSERT(fmi::cmp(fmi::pow(3., 0.), 1.));
	ASSERT(fmi::cmp(fmi::pow(3., 1.), 3.));
	ASSERT(fmi::cmp(fmi::pow(1., 0.), 1.));
	ASSERT(fmi::cmp(fmi::pow(3., 2.), 9.));
	ASSERT(fmi::cmp(fmi::pow(45, 4), 4100625));
	ASSERT(fmi::cmp(fmi::pow(3., -1.), 1./3.));

	// N-th root
	ASSERT(fmi::cmp(fmi::n_root(4., 2), 2.));
	ASSERT(fmi::cmp(fmi::n_root(4, 2), 2));
	//ASSERT(fmi::cmp(fmi::n_root(5., 2), 2.23607));
	ASSERT(fmi::cmp(fmi::n_root(5, 2), 2));

	ASSERT(fmi::cmp(fmi::sqrt(2.f), 1.414213562f));

	// Exponentiation
	ASSERT(fmi::cmp(fmi::exp(2), 7));
	ASSERT(fmi::cmp(fmi::exp(2.f), 7.389056099f));

	// Clamp
	ASSERT(fmi::cmp(fmi::clamp(3.5, 1., 3.), 3.));

	// Sine
	ASSERT(fmi::cmp(fmi::sin(0.), 0.));
	ASSERT(fmi::cmp(fmi::sin(1.570796327f), 1.f));
	std::cout << fmi::sin(1.570796327f) << '\n';

	// Cosine
	ASSERT(fmi::cmp(fmi::cos(0.), 1.));
	//ASSERT(fmi::cmp(fmi::cos(1.570796327f), 0.f));

	// Tangent
	ASSERT(fmi::cmp(fmi::tan(0.), 0.));

	// Arcsine
	ASSERT(fmi::cmp(fmi::asin(0.), 0.));

	// Arccosine
	ASSERT(fmi::cmp(fmi::acos(1.), 0.));

	// Arctangent 
	ASSERT(fmi::cmp(fmi::atan(0.), 0.));

	// Arctangent (2)
	//ASSERT(fmi::cmp(fmi::atan2(1., 1.), 0.785398163));
}
