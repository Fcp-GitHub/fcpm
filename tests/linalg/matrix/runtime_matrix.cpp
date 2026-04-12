#include "linalg/matrix.hpp"

#include <iostream>
#include <numeric>

using type = int;
using mat_t = fcp::math::Matrix<type, 4, 4>;

int main()
{
	namespace fcpm = fcp::math;
	using namespace fcpm::operators;

	mat_t matrix{};

	std::iota(matrix.begin(), matrix.end(), 0);
	std::cout << "Matrix: " << matrix << '\n';

	auto row_iter{ matrix.row(3) };
	std::iota(row_iter.begin(), row_iter.end(), 0);

	std::cout << "Matrix: " << matrix << '\n';

	for (auto& elem : matrix.row(2))
		elem += 3;

	std::cout << "Matrix: " << matrix << '\n';
	
	mat_t mat_ls{
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4
	};

	mat_t mat_ls_temp{
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4
	};

	// Test Matrix transposition
	auto mat_ls_t{ mat_ls_temp.transpose() };
	auto transposed{ mat_ls_temp.transpose() };

	std::cout << "\nOriginal: \n" << mat_ls << '\n';
	std::cout << "Tranposed: \n" << transposed << '\n';

	// Swap rows/columns
	
	mat_t mat_ls_swap_row{
		3, 3, 3, 3,
		2, 2, 2, 2,
		1, 1, 1, 1,
		4, 4, 4, 4
	};

	auto mat_swap_rows_02_from_expr{ transposed.swap_rows(0, 2) };
	auto mat_swap_rows_02_from_mat{ mat_ls_t.swap_rows(0, 2) };

	std::cout << "\nSwap rows 0,2 (transposed):\n";
	std::cout << mat_ls_swap_row << '\n';
	std::cout << mat_swap_rows_02_from_mat << '\n';

	mat_t mat_ls_swap_col{
		3, 2, 1, 4,
		3, 2, 1, 4,
		3, 2, 1, 4,
		3, 2, 1, 4
	};

	auto mat_swap_cols_02_from_mat{ mat_ls_temp.swap_cols(0, 2) };

	std::cout << "\nSwap columns 0,2 (original):\n";
	std::cout << mat_ls_swap_col << '\n';
	std::cout << mat_swap_cols_02_from_mat << '\n';

	// Matrix multiplication
	
	fcpm::Matrix<type, 8, 6> medium_matrix(10);

	auto mat_gemm{ medium_matrix * medium_matrix.transpose() };	
	
	std::cout << "\nMedium matrix:\n" << medium_matrix << '\n';
	std::cout << mat_gemm << '\n';

	return 0;
}
