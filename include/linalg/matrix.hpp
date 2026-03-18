#ifndef FCP_MATH_LINALG_MATRIX_PUBLIC_HPP
#define FCP_MATH_LINALG_MATRIX_PUBLIC_HPP

//----------------------------------------------------------------------------------
// Include header files
//----------------------------------------------------------------------------------

#include "internal/scalar.hpp"
#include "internal/matrix_class.hpp"
#include "internal/matrix_operations.hpp"
#include "internal/vector_operations.hpp"

// Make operators public
using namespace fcp::math::operators;

//----------------------------------------------------------------------------------
// Convenient type aliases
//----------------------------------------------------------------------------------

template <typename T> using RowVec2 = fcp::math::RowVector<T, 2>;
template <typename T> using RowVec3 = fcp::math::RowVector<T, 3>;
template <typename T> using RowVec4 = fcp::math::RowVector<T, 4>;

template <typename T> using ColVec2 = fcp::math::ColumnVector<T, 2>;
template <typename T> using ColVec3 = fcp::math::ColumnVector<T, 3>;
template <typename T> using ColVec4 = fcp::math::ColumnVector<T, 4>;

#endif	//FCP_MATH_LINALG_MATRIX_PUBLIC_HPP
