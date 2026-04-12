#include "linalg/matrix.hpp"
#include "linalg/solvers/lu_solver.hpp"

#include <iostream>

namespace fcpm = fcp::math;

using mat_t = fcpm::Matrix<double, 3, 3, fcp::math::RowMajor | fcp::math::StaticStorage>;
using vec_t = fcpm::ColumnVector<double, 3>;

int main()
{
	constexpr static mat_t A{
		1., 2., 1.,
		2., 6., 1.,
		1., 1., 4.
	};
	
	constexpr static vec_t b0{ 2., 7., 3. };
	constexpr static vec_t b1{ 1., 3., 2. };
	constexpr static vec_t b2{ 14., 16., 42. };
	constexpr static vec_t b3{ 24., 36., 81. };

	constexpr static fcpm::solvers::LUSolver solver(A);
	
	static_assert(solver.solve(b0) == vec_t{-3., 2., 1.});

	std::cout << solver.solve(b0) << '\n';
	std::cout << solver.solve(b1) << '\n';
	std::cout << solver.solve(b2) << '\n';
	std::cout << solver.solve(b3) << '\n';

	std::cout << '\n';

	std::cout << solver.get_determinant() << '\n';
	std::cout << solver.get_inverse() << '\n';
}
