#include "linalg/matrix.hpp"
#include "linalg/solvers/dense_solvers.hpp"

#include <iostream>

using mat_t = fcp::math::Matrix<double, 4, 4, fcp::math::ColumnMajor | fcp::math::StaticStorage>;
using vec_t = fcp::math::ColumnVector<double, 4>;

int main()
{
	
	//----------------------------------------------------------------------------------
	// Backward/Forward substitution
	//----------------------------------------------------------------------------------

	constexpr static mat_t U{
		4., 5., 3., 1.,
		0., 2., 1., 3.,
		0., 0., 1., 2.,
		0., 0., 0., 2.
	};

	constexpr static mat_t L{
		1., 0., 0., 0.,
		5., 4., 0., 0.,
		3., 7., 1., 0.,
		3., 4., 2., 6.
	};

	constexpr static vec_t b{ 15., 45., 8., 4. };

	constexpr static vec_t x{ 
		fcp::math::solvers::backward_substitution(U, b) 
	};

	constexpr static vec_t y{
		fcp::math::solvers::forward_substitution(L, b)
	};

	static_assert((U*x) == b);
	
	std::cout << "U:\n" << U << '\n';
	std::cout << "b:\n" << b << '\n';
	std::cout << "x:\n" << x << '\n';

	constexpr auto prodUx{ (U*x) };

	std::cout << "Ux:\n" << prodUx << '\n';

	static_assert((L*y) == b);

	std::cout << "L:\n" << L << '\n';
	std::cout << "b:\n" << b << '\n';
	std::cout << "y:\n" << y << '\n';

	constexpr auto prodLy{ (L*y) };

	std::cout << "Ly:\n" << prodLy << '\n';

	//----------------------------------------------------------------------------------
	// Gauss elimination
	//----------------------------------------------------------------------------------
	
	mat_t M{
		1., 2., 1., 3.,
		2., 6., 1., 4.,
		1., 1., 4., 3.,
		9., 5., 7., 2.
	};

	vec_t b0(b);

	std::cout << "M:\n" << M << '\n';
	std::cout << "b:\n" << b0 << '\n';

	vec_t res{
		fcp::math::solvers::gauss_elimination(M, b0)
	};

	std::cout << "M:\n" << M << '\n';
	std::cout << "b:\n" << b0 << '\n';
	std::cout << "res:\n" << res << '\n';

	auto prodMres{ (M*res) };

	std::cout << "Mres:\n" << prodMres << '\n';
}
