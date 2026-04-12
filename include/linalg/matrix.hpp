#ifndef FCP_MATH_LINALG_MATRIX_PUBLIC_HPP
#define FCP_MATH_LINALG_MATRIX_PUBLIC_HPP

//----------------------------------------------------------------------------------
// Include header files
//----------------------------------------------------------------------------------

#include "core/common.hpp"
#include "core/hardware.hpp"
#include "core/operators.hpp"

#include "linalg/internal/matrix_class.hpp"
#include "linalg/internal/matrix_operations.hpp"
#include "linalg/internal/vector_operations.hpp"

// Make operators public
using namespace fcp::math::operators;

//----------------------------------------------------------------------------------
// Convenient type aliases
//----------------------------------------------------------------------------------

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

template <typename T> using RowVec2 = fcp::math::RowVector<T, 2>;
template <typename T> using RowVec3 = fcp::math::RowVector<T, 3>;
template <typename T> using RowVec4 = fcp::math::RowVector<T, 4>;

template <typename T> using ColVec2 = fcp::math::ColumnVector<T, 2>;
template <typename T> using ColVec3 = fcp::math::ColumnVector<T, 3>;
template <typename T> using ColVec4 = fcp::math::ColumnVector<T, 4>;

// Column-major, static storage matrix
template <typename T, int NumRows, int NumColumns> 
using ColMatrix = fcp::math::Matrix<
	T, NumRows, NumColumns, 
	fcp::math::ColumnMajor | fcp::math::StaticStorage
>;

// Row-major, static storage matrix (it's still the default one, just for clarity if desired)
template <typename T, int NumRows, int NumColumns>
using RowMatrix = fcp::math::Matrix<T, NumRows, NumColumns>;

//TODO: for graphics module
template <typename T, int NumRows, int NumColumns>
using DefaultMatrixType = std::conditional_t<
	FCPM_GRAPHICS_USE_ROW_MAJOR,
	RowMatrix<T, NumRows, NumColumns>,
	ColMatrix<T, NumRows, NumColumns>
>;

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_MATRIX_PUBLIC_HPP
