#include "linalg/matrix.hpp"
#include "geometry/transforms.hpp"

#include <iostream>
#include <string_view>

#define QUALIFIERS constexpr static

namespace fcpm = fcp::math;

constexpr void test_projection(std::string_view title, const auto& proj_matrix)
{
	QUALIFIERS fcpm::ColVec4<double> lpvec{ -1., -1., 10., 1. };
	QUALIFIERS fcpm::ColVec4<double> origin{ 0., 0., 0., 1. };

	std::cout << "======== " << title << " ========\n";	
	std::cout << "matrix:\n" << proj_matrix << '\n';
	std::cout << "Random vector: " << lpvec << '\n';
	std::cout << "Projection: " << (proj_matrix * lpvec).eval() << '\n';
	std::cout << std::endl;

	std::cout << "Origin vector: " << origin << '\n';
	std::cout << "Projection: " << (proj_matrix * origin).eval() << '\n';
	std::cout << std::endl;
}

int main()
{
	// 1. Orthographic projection	

	QUALIFIERS fcpm::ColMatrix<double, 4, 4> ortho_std{
		fcpm::ortho(-2., 5., -4., 3., 0.1, 100.)
	};	
	test_projection("STANDARD ORTHOGONAL PROJECTION", ortho_std);


	QUALIFIERS fcpm::ColMatrix<double, 4, 4> ortho_sym{
		fcpm::ortho_sym(1., 1., 0.1, 100.)
	};
	test_projection("SYMMETRIC ORTHOGONAL PROJECTION", ortho_sym);


	// 2. Perspective projection

	QUALIFIERS fcpm::ColMatrix<double, 4, 4> persp_std{
		fcpm::persp(-2., 5., -4., 3., 0.1, 100.)
	};
	test_projection("PERSPECTIVE PROJECTION WITH CUSTOM FRUSTUM", persp_std);

	QUALIFIERS fcpm::ColMatrix<double, 4, 4> persp_inf{
		fcpm::persp_inf(-2., 5., -4., 3., 0.1)	
	};
	test_projection("INFINITE PERSPECTIVE PROJECTION WITH CUSTOM FRUSTUM", persp_inf);

	QUALIFIERS fcpm::ColMatrix<double, 4, 4> persp_sym{
		fcpm::persp_sym(std::numbers::pi_v<double>/6., 800./600., 0.1, 100.)
	};
	test_projection("SYMMETRIC PERSPECTIVE PROJECTION (FOV, ASPECT RATIO)", persp_sym);

	QUALIFIERS fcpm::ColMatrix<double, 4, 4> persp_inf_sym{
		fcpm::persp_inf_sym(std::numbers::pi_v<double>/6., 800./600., 0.1)
	};
	test_projection("INFINITE SYMMETRIC PERSPECTIVE PROJECTION (FOV, ASPECT RATIO)", persp_inf_sym);

	// 3. LookAt algorithm

	QUALIFIERS fcpm::ColVec3<double> eye{ 0., 0., 5. };
	QUALIFIERS fcpm::ColVec3<double> at { 0., 0., 0. };
	QUALIFIERS fcpm::ColVec3<double> wup{ 0., 1., 0. };

	std::cout << "======== LOOKAT PARAMETERS ========\n";
	std::cout << "eye = " << eye << '\n';
	std::cout << "at  = " << at  << '\n';
	std::cout << "world_up = " << wup << '\n';
	std::cout << std::endl;

	QUALIFIERS fcpm::ColMatrix<double, 4, 4> view{
		fcpm::look_at(eye, at, wup)
	};
	test_projection("LOOKAT CONVERSION", view);

	// 4. Final frustum test
	test_projection("FINAL FRUSTUM (SYMMETRIC PERSPECTIVE PROJECTION + LOOKAT)", (persp_sym * view).eval());
}
