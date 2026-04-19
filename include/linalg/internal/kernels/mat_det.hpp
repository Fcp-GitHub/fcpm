#ifndef FCP_MATH_LINALG_INTERNAL_KERNELS_MATDET_HPP
#define FCP_MATH_LINALG_INTERNAL_KERNELS_MATDET_HPP

#include "core/internal/common.hpp"

#include "linalg/solvers/lu_solver.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE
START_FCP_KERNELS_NAMESPACE

template <LazyMatrixType M>
constexpr auto determinant_dispatcher(M& matrix)
{
	using traits = Traits<std::remove_cvref_t<M>>;

	constexpr int rows{ traits::rows };
	constexpr int cols{ traits::cols };

	static_assert(rows == cols, "Determinant available only for square matrices.");

	if constexpr (rows <= 4)
	{
		return determinant_tiny_solver<rows>(matrix);
	} else {

		//TODO: factory method?
		solvers::LUSolver solver(matrix);

		return solver.get_determinant();
	}
}

template <int N>
constexpr auto determinant_tiny_solver(const auto& matrix)
{
	if constexpr (N == 2) return get_determinant_2x2(matrix);
	else if constexpr (N == 3) return get_determinant_3x3(matrix);
	return get_determinant_4x4(matrix);
}

template <LazyMatrixType M>
constexpr auto get_determinant_2x2(const M& m)
{
 return m[0, 0]*m[1, 1] - m[0, 1]*m[1, 0];
}

template <LazyMatrixType M>
constexpr auto get_determinant_3x3(const M& m)
{
	using mat_t = std::remove_cvref_t<M>;
	using T = Traits<mat_t>::element_type;

	T temp0{ m[1, 1]*m[2, 2] - m[1, 2]*m[2, 1] };
	T temp1{ m[1, 2]*m[2, 0] - m[1, 0]*m[2, 2] };
	T temp2{ m[1, 0]*m[2, 1] - m[1, 1]*m[2, 0] };

	return m[0, 0]*temp0 + m[0, 1]*temp1 + m[0, 2]*temp2;
}

template <LazyMatrixType M>
constexpr auto get_determinant_4x4(const M& m)
{
  // Pre-compute 2x2 matrix minors
  double s0 = m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
  double s1 = m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0);
  double s2 = m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0);
  double s3 = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);
  double s4 = m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1);
  double s5 = m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2);

  double v0 = m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0);
  double v1 = m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0);
  double v2 = m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0);
  double v3 = m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1);
  double v4 = m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1);
  double v5 = m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2);

  return s0 * v5 - s1 * v4 + s2 * v3 + s3 * v2 - s4 * v1 + s5 * v0;
}

END_FCP_KERNELS_NAMESPACE
END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_INTERNAL_KERNELS_MATDET_HPP
