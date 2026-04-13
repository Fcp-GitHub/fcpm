#include "linalg/matrix.hpp"
#include "geometry/conversions.hpp"
#include "geometry/quaternion.hpp"
#include "geometry/transforms.hpp"

#include <iostream>
#include <numbers>

#define QUALIFIER const

namespace fcpm = fcp::math;

using type = double;

void test_std(const auto& first, const auto& result)
{
	std::cout << "Initial data: " << first << '\n';
	std::cout << "Rotated data: " << result << '\n';
	std::cout << std::endl;
}

int main()
{
	constexpr auto pio2{ std::numbers::pi_v<type>/static_cast<type>(2.) };

	QUALIFIER std::array<type, 3> euler{
		static_cast<type>(0.), 													// HEADING	(Y)
		pio2,																					  // PITCH 		(X)
		static_cast<type>(0.)														// BANK			(Z)
	};
	QUALIFIER static fcpm::ColVec3<type> vec{ static_cast<type>(0.), static_cast<type>(1.), static_cast<type>(0.) };	

	// Rotations using quaternions
	QUALIFIER static fcpm::UnitQuaternion<type> quat{ 
		fcpm::UnitQuaternion<type>::from_euler(euler).renormalize()
	};	
	std::cout << "quat: " << quat << '\n';

	QUALIFIER static fcpm::Quaternion<type> gquat{
		fcpm::Quaternion<type>::from_euler(euler)
	};
	std::cout << "gquat: " << gquat << '\n';
	std::cout << std::endl;

	test_std(vec, fcpm::rotate(vec, quat));
	test_std(vec, fcpm::rotate(vec, gquat));

	// Rotation with matrices
	QUALIFIER static fcpm::ColMatrix<type, 4, 4> mat{
		fcpm::ColMatrix<type, 4, 4>::identity()
	};
	std::cout << "Matrix:\n" << mat << '\n';

	fcpm::ColVec3<type> n{ static_cast<type>(0) };
	fcpm::ColVec4<type> v4{ vec };
	n[0] = static_cast<type>(1);	
	std::cout << fcpm::rotate(mat, pio2, n) << '\n';
	test_std((mat*v4).eval(), (fcpm::rotate(mat, pio2, n)*v4).eval()); 

	// Rotation with Euler angles
	test_std(vec, fcpm::rotate(euler[0], euler[1], euler[2], vec));
}
