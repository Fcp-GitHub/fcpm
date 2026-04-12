#include "core/common.hpp"
#include "linalg/matrix.hpp"
#include "geometry/transforms.hpp"

#include <iostream>

#define QUALIFIER constexpr

namespace fcpm = fcp::math;

void test_identity(const auto& matrix)
{
	std::cout << "Initial matrix:\n" << matrix << '\n';	
	std::cout << std::endl;
}

void test_inversion(const auto& first, const auto& inverse)
{
	std::cout << "Initial data:\n" << first << '\n';
	std::cout << "Inverse data:\n" << inverse << '\n';
	std::cout << "Inital + inverse:\n" << first * inverse << '\n';
	std::cout << std::endl;
}

int main()
{
	QUALIFIER static fcpm::ColMatrix<double, 4, 4> mat{ fcpm::ColMatrix<double, 4, 4>::identity() };

	std::cout << "====== IDENTITY TESTS ======\n\n";

	// Identity reflection
	test_identity(fcpm::reflect<double>(mat, fcpm::ColumnVector<double, 3>(1., 0., 0.)));

	// Identity translation
	test_identity(fcpm::translate<double>(mat, fcpm::ColumnVector<double, 3>(0.)));

	// Uniform identity scaling
	test_identity(fcpm::scale<double>(mat, fcpm::ColumnVector<double, 3>(1.)));

	// Inverse translation
	test_inversion(
		fcpm::translate<double>(mat, fcpm::ColumnVector<double, 3>(1.)), 
		fcpm::translate<double>(mat, fcpm::ColumnVector<double, 3>(-1.))
	);

	// Inverse uniform scaling
	test_inversion(
		fcpm::scale<double>(mat, fcpm::ColumnVector<double, 3>(2.)),
		fcpm::scale<double>(mat, fcpm::ColumnVector<double, 3>(0.5))
	);
}
