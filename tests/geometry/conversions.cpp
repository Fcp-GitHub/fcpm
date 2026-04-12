#include "core/math_fun.hpp"
#include "linalg/matrix.hpp"
#include "geometry/conversions.hpp"
#include "geometry/quaternion.hpp"

#include <cassert>
#include <numbers>
#include <array>
#include <iostream>

#define ASSERT(x) assert((x))
#define QUALIFIER const

namespace fcpm = fcp::math;

std::ostream& operator<<(std::ostream& out, const std::array<double, 3> a)
{
	out << '[' << a[0] << ", " << a[1] << ", " << a[2] << ']';
	return out;
}

template <typename T>
constexpr void test_conversions(
	std::string_view title,
	const std::array<T, 3>& euler,
	const fcpm::ColMatrix<T, 4, 4>& matrix,
	const fcpm::UnitQuaternion<T>& quaternion
)
{
	std::cout << "========= " << title << " =========" << '\n';
	std::cout << "INITIAL DATA:\n";
	std::cout << "Euler angles: " << euler << '\n';
	std::cout << "Matrix: " << matrix << '\n';
	std::cout << "Quaternion: " << quaternion << '\n';
	std::cout << std::endl;

	// 1. Euler - Matrix
	std::cout << "EULER - MATRIX\n";
	// Euler from Matrix
	QUALIFIER auto e_from_mat{ fcpm::euler_from_matrix(matrix) };
	std::cout << "Euler angles from matrix: " << e_from_mat << '\n';
	std::cout << "Re-computed matrix:\n" << fcpm::matrix_from_euler(e_from_mat) << '\n';
	// Matrix from Euler
	QUALIFIER auto mat_from_e{ fcpm::matrix_from_euler(euler) };
	std::cout << "Matrix from Euler angles:\n" << mat_from_e << '\n';
	std::cout << "Re-computed Euler angles: " << fcpm::euler_from_matrix(mat_from_e) << '\n';
	std::cout << std::endl;

	// 2. Euler - Quaternion
	std::cout << "EULER - QUATERNION\n";
	// Euler from Quaternion
	QUALIFIER auto e_from_q{ quaternion.to_euler() };
	std::cout << "Euler angles from quaternion: " << e_from_q << '\n';
	std::cout << "Re-computed quaternion: " << fcpm::UnitQuaternion<T>::from_euler(e_from_q) << '\n';
	// Quaternion from Euler
	QUALIFIER auto q_from_e{ fcpm::UnitQuaternion<T>::from_euler(euler) };
	std::cout << "Quaternion from Euler angles: " << q_from_e << '\n';
	std::cout << "Re-computed Euler angles: " << q_from_e.to_euler() << '\n';
	std::cout << std::endl;

	// 3. Quaternion - Matrix
	std::cout << "QUATERNION - MATRIX\n";
	// Quaternion from matrix
	QUALIFIER auto q_from_m{ fcpm::UnitQuaternion<T>::from_matrix(matrix) }; 
	std::cout << "Quaternion from matrix: " << q_from_m << '\n';
	std::cout << "Re-computed matrix:\n" << q_from_m.to_matrix() << '\n';
	// Matrix from Quaternion
	QUALIFIER auto m_from_q{ quaternion.to_matrix() };
	std::cout << "Matrix from quaternion:\n" << m_from_q << '\n';
	std::cout << "Re-computed quaternion: " << fcpm::UnitQuaternion<T>::from_matrix(m_from_q) << '\n'; 
	std::cout << std::endl;
	std::cout << std::endl;
}

int main()
{
	// Utility
	QUALIFIER auto pio6{ std::numbers::pi_v<double>/6. };
	QUALIFIER auto pio4{ std::numbers::pi_v<double>/4. };
	QUALIFIER auto pio2{ std::numbers::pi_v<double>/2. };
	QUALIFIER auto cospio6{ fcpm::cos(pio6) };
	QUALIFIER auto sinpio6{ fcpm::sin(pio6) };

	// Conversion from radians to degrees
	ASSERT(fcpm::cmp(fcpm::rad_to_deg(0.), 0.));	
	ASSERT(fcpm::cmp(fcpm::rad_to_deg(std::numbers::pi_v<double>), 180.));

	// Conversion from degrees to radians
	ASSERT(fcpm::cmp(fcpm::deg_to_rad(0.), 0.));
	ASSERT(fcpm::cmp(fcpm::deg_to_rad(180.), std::numbers::pi_v<double>));

	// Test standard case
	QUALIFIER static fcpm::ColMatrix<double, 4, 4> matrix{ 
		cospio6, -sinpio6, 0., 0.,
		sinpio6, cospio6, 0., 0.,
		0., 0., 1., 0.,
		0., 0., 0., 1.
	};	
	QUALIFIER std::array<double, 3> euler{ 0., 0., pio6 };
	QUALIFIER static fcpm::UnitQuaternion<double> quaternion{ 0.965926, 0., 0., 0.258819 };

	test_conversions("STANDARD", euler, matrix, quaternion);

	// Test gimbal lock case
	QUALIFIER static fcpm::ColMatrix<double, 4, 4> matrix_gl{
		0.707, 0., 0.707, 0.,
		0.707, 0., -0.707, 0.,
		0., 1., 0., 0.,
		0., 0., 0., 1.
	};
	QUALIFIER std::array<double, 3> euler_gl{ pio2, pio4, pio2 };
	QUALIFIER static fcpm::UnitQuaternion<double> quaternion_gl{ 0.6532899, 0.6532899, 0.2705776, 0.2705776 };

	test_conversions("GIMBAL LOCK", euler_gl, matrix_gl, quaternion_gl);
}
