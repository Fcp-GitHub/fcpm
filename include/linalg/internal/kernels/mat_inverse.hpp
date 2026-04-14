#ifndef FCP_MATH_LINALG_INTERNAL_KERNELS_MATINVERSE_HPP
#define FCP_MATH_LINALG_INTERNAL_KERNELS_MATINVERSE_HPP

#include "core/common.hpp"

#include "linalg/solvers/lu_solver.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE
START_FCP_KERNELS_NAMESPACE

template <LazyMatrixType M>
constexpr void get_inverse_2x2(auto& result, const M& m)
{
	using mat_t = std::remove_cvref_t<M>;
	using T = Traits<mat_t>::element_type;

	const auto inv_det{ static_cast<T>(1) / (m[0, 0]*m[1, 1] - m[0, 1]*m[1, 0]) };

	result[0, 0] = m[1, 1]*inv_det;
	result[0, 1] = -m[0, 1]*inv_det;
	result[1, 0] = -m[1, 0]*inv_det;
	result[1, 1] = m[0, 0]*inv_det;
}

template <LazyMatrixType M>
constexpr void get_inverse_3x3(auto& result, const M& m)
{
	using mat_t = std::remove_cvref_t<M>;
	using T = Traits<mat_t>::element_type;

	result[0, 0] = (m[1, 1]*m[2, 2] - m[1, 2]*m[2, 1]);	
	result[1, 0] = (m[1, 2]*m[2, 0] - m[1, 0]*m[2, 2]);
	result[2, 0] = (m[1, 0]*m[2, 1] - m[1, 1]*m[2, 0]);

	const auto inv_det{ 
		static_cast<T>(1) / 
		(m[0, 0]*result[0, 0] + m[0, 1]*result[1, 0] + m[0, 2]*result[2, 0]) 
	};

	result[0, 0] *= inv_det;
	result[1, 0] *= inv_det;
	result[2, 0] *= inv_det;
	result[0, 1] = (m[0, 2]*m[2, 1] - m[0, 1]*m[2, 2]) * inv_det;
	result[1, 1] = (m[0, 0]*m[2, 2] - m[0, 2]*m[2, 0]) * inv_det;
	result[2, 1] = (m[2, 0]*m[0, 1] - m[0, 0]*m[2, 1]) * inv_det;
	result[0, 2] = (m[0, 1]*m[1, 2] - m[0, 2]*m[1, 1]) * inv_det;
	result[1, 2] = (m[1, 0]*m[0, 2] - m[0, 0]*m[1, 2]) * inv_det;
	result[2, 2] = (m[0, 0]*m[1, 1] - m[1, 0]*m[0, 1]) * inv_det;
}

template <LazyMatrixType M>
constexpr void get_inverse_4x4(auto& result, const M& m)
{
	using mat_t = std::remove_cvref_t<M>;
  using T = Traits<mat_t>::element_type;
  
  const T b00{ m[2, 2] * m[3, 3] - m[2, 3] * m[3, 2] };
  const T b01{ m[2, 1] * m[3, 3] - m[2, 3] * m[3, 1] };
  const T b02{ m[2, 1] * m[3, 2] - m[2, 2] * m[3, 1] };
  const T b03{ m[2, 0] * m[3, 3] - m[2, 3] * m[3, 0] };
  const T b04{ m[2, 0] * m[3, 2] - m[2, 2] * m[3, 0] };
  const T b05{ m[2, 0] * m[3, 1] - m[2, 1] * m[3, 0] };
  const T b06{ m[1, 2] * m[3, 3] - m[1, 3] * m[3, 2] };
  const T b07{ m[1, 1] * m[3, 3] - m[1, 3] * m[3, 1] };
  const T b08{ m[1, 1] * m[3, 2] - m[1, 2] * m[3, 1] };
  const T b09{ m[1, 0] * m[3, 3] - m[1, 3] * m[3, 0] };
  const T b10{ m[1, 0] * m[3, 2] - m[1, 2] * m[3, 0] };
  const T b11{ m[1, 0] * m[3, 1] - m[1, 1] * m[3, 0] };
  const T b12{ m[1, 2] * m[2, 3] - m[1, 3] * m[2, 2] };
  const T b13{ m[1, 1] * m[2, 3] - m[1, 3] * m[2, 1] };
  const T b14{ m[1, 1] * m[2, 2] - m[1, 2] * m[2, 1] };
  const T b15{ m[1, 0] * m[2, 3] - m[1, 3] * m[2, 0] };
  const T b16{ m[1, 0] * m[2, 2] - m[1, 2] * m[2, 0] };
  const T b17{ m[1, 0] * m[2, 1] - m[1, 1] * m[2, 0] };

  // Compute cofactors
  const T c00{   m[1, 1] * b00 - m[1, 2] * b01 + m[1, 3] * b02  };
  const T c01{ -(m[1, 0] * b00 - m[1, 2] * b03 + m[1, 3] * b04) };
  const T c02{   m[1, 0] * b01 - m[1, 1] * b03 + m[1, 3] * b05  };
  const T c03{ -(m[1, 0] * b02 - m[1, 1] * b04 + m[1, 2] * b05) };

	// Re-compute determinant for two main reasons (micro-optimization):
	// 1. The determinant has many common factors (avoid redundant calculations)
	// 2. Avoid function call
  const T det{ m[0, 0] * c00 + m[0, 1] * c01 + m[0, 2] * c02 + m[0, 3] * c03 };
  
  //TODO: Check for singularity?
  //if (cmp(det, T{0})) {
  //    throw std::runtime_error("Matrix is singular");
  //}

  const T inv_det{ static_cast<T>(1) / det };

  result[0, 0] = c00 * inv_det;
  result[1, 0] = c01 * inv_det;
  result[2, 0] = c02 * inv_det;
  result[3, 0] = c03 * inv_det;

  result[0, 1] = -(m[0, 1] * b00 - m[0, 2] * b01 + m[0, 3] * b02) * inv_det;
  result[1, 1] =  (m[0, 0] * b00 - m[0, 2] * b03 + m[0, 3] * b04) * inv_det;
  result[2, 1] = -(m[0, 0] * b01 - m[0, 1] * b03 + m[0, 3] * b05) * inv_det;
  result[3, 1] =  (m[0, 0] * b02 - m[0, 1] * b04 + m[0, 2] * b05) * inv_det;

  result[0, 2] =  (m[0, 1] * b06 - m[0, 2] * b07 + m[0, 3] * b08) * inv_det;
  result[1, 2] = -(m[0, 0] * b06 - m[0, 2] * b09 + m[0, 3] * b10) * inv_det;
  result[2, 2] =  (m[0, 0] * b07 - m[0, 1] * b09 + m[0, 3] * b11) * inv_det;
  result[3, 2] = -(m[0, 0] * b08 - m[0, 1] * b10 + m[0, 2] * b11) * inv_det;

  result[0, 3] = -(m[0, 1] * b12 - m[0, 2] * b13 + m[0, 3] * b14) * inv_det;
  result[1, 3] =  (m[0, 0] * b12 - m[0, 2] * b15 + m[0, 3] * b16) * inv_det;
  result[2, 3] = -(m[0, 0] * b13 - m[0, 1] * b15 + m[0, 3] * b17) * inv_det;
  result[3, 3] =  (m[0, 0] * b14 - m[0, 1] * b16 + m[0, 2] * b17) * inv_det;
}


template <int N>
constexpr void inverse_tiny_solver(auto& result, const auto& matrix)
{
	if constexpr (N == 2) get_inverse_2x2(result, matrix);
	else if constexpr (N == 3) get_inverse_3x3(result, matrix);
	else get_inverse_4x4(result, matrix);
}


template <LazyMatrixType R, LazyMatrixType M>
constexpr void inverse_dispatcher(R& result, const M& matrix)
{
	using traits = Traits<std::remove_cvref_t<M>>;

	constexpr int rows{ traits::rows };
	constexpr int cols{ traits::columns };

	static_assert(rows == cols, "Matrix inverse available only for square matrices.");

	if constexpr (rows <= 4)
	{
		inverse_tiny_solver<rows>(result, matrix);
	} else {
		
		//TODO: factory method?
		solvers::LUSolver solver(matrix);

		result = solver.get_inverse();
	}
}

END_FCP_KERNELS_NAMESPACE
END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_INTERNAL_KERNELS_MATINVERSE_HPP
