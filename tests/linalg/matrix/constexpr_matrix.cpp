#include "linalg/matrix.hpp"

#include "test_utils/ansi_seq.hpp"

#include <iostream>

using type = int;
using mat_t = fcp::math::Matrix<type, 4, 4>;

int main()
{
	namespace fcpm = fcp::math;
	using namespace fcpm::operators;

	//----------------------------------------------------------------------------------
	// Constructors
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Constructors --\n";

	// Default constructor:
	// 1. Default initialization: no, cannot have non-initialized variables
	// 														at compile-time. Furthermore, since compile-time
	// 														programming is based on immutables, I don't
	// 														think it would be even needed
	//constexpr fcpm::Matrix<type, 4, 4> mat_d;
	//
	// 2. Value initialization
	constexpr static mat_t mat_v{};

	// Create a matrix using a list of vectors
	// 1. Temporaries
	constexpr static fcpm::Matrix<type, 4, 4> mat_lt{
		fcpm::RowVector<type, 4>{1, 2, 3, 4},
		fcpm::RowVector<type, 4>{1, 2, 3, 4},
		fcpm::RowVector<type, 4>{1, 2, 3, 4},
		fcpm::RowVector<type, 4>{1, 2, 3, 4}
	};		

	// 2. References
	constexpr static fcpm::RowVector<type, 4> vec{ 1, 2, 3, 4 };
	constexpr static fcpm::Matrix<type, 4, 4> mat_lr{
		vec, vec, vec, vec
	};

	// Broadcasting constructor
	constexpr static mat_t mat_brd(1);

	fcp::math::for_constexpr<0, mat_brd.rows()>([&](auto i){
			fcp::math::for_constexpr<0, mat_brd.columns()>([&](auto j){
				static_assert(mat_brd[i, j] == 1);
			});
	});


	// Create a matrix using a list of scalars
	constexpr static mat_t mat_ls{
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4
	};

	constexpr static fcpm::Matrix<type, 4, 4, fcpm::ColumnMajor | fcpm::StaticStorage> 
	mat_ls_cm{
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4,
		1, 2, 3, 4
	};

	static_assert(mat_ls[0, 3] == mat_ls_cm[0, 3]);

	// Copy constructor
	constexpr static mat_t mat_c{ mat_ls };

	static_assert(mat_c == mat_ls);

	// Const vector: not tested, constexpr implies const

	// Test accessor method used to get underlying buffer
	constexpr static auto buf{ mat_ls.data() };

	// Test identity matrix factory method
	constexpr static auto identity{ mat_t::identity() };

	static_assert(mat_ls == (mat_ls*identity));

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Arithmetic operators
	//----------------------------------------------------------------------------------

	std::cout << "-- Arithmetic operators --\n";
	
	constexpr auto mat_add{ mat_v + mat_ls };

	static_assert(mat_add[0, 0] == 1);
	static_assert(mat_add[2, 1] == 2);

	mat_t m = mat_add;		
	
	constexpr auto op1{ mat_add + mat_ls + mat_add };
	constexpr auto op2{ op1 + mat_ls + mat_add };

	static_assert(op1[2, 1] == 6);
	static_assert(op2[2, 1] == 10);

	constexpr auto mat_sub{ mat_ls - mat_c };

	static_assert(mat_sub[10] == 0);

	constexpr auto mat_brd_mul{ mat_ls * 3 };

	static_assert(mat_brd_mul[0, 0] == 3);

	constexpr auto mat_brd_add{ mat_ls + 4 };

	static_assert(mat_brd_add[0] == 5);

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Logical operators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Logical operators --\n";
	
	static_assert(mat_ls == mat_c);
	static_assert(!(mat_ls != mat_c));
	
	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Matrix operations
	//----------------------------------------------------------------------------------

	constexpr auto mat_mul{ mat_ls * mat_ls };

	constexpr static mat_t mat_mul_res{
		10, 20, 30, 40,
		10, 20, 30, 40,
		10, 20, 30, 40,
		10, 20, 30, 40
	};

	fcp::math::for_constexpr<0, mat_mul.rows()>([&](auto i){
			fcp::math::for_constexpr<0, mat_mul.columns()>([&](auto j){
				static_assert(mat_mul[i, j] == mat_mul_res[i, j]);
			});
	});
				
	static_assert(mat_mul[0, 0] == mat_mul_res[0, 0]);
	
	static_assert(mat_mul == mat_mul_res);

	constexpr static mat_t mat_ls_t{
		1, 1, 1, 1,
		2, 2, 2, 2,
		3, 3, 3, 3,
		4, 4, 4, 4
	};

	// Test Matrix transposition
	constexpr auto transposed{ mat_ls.transpose() };

	static_assert(mat_ls_t == transposed);

	// Swap rows/columns
	
	constexpr static mat_t mat_ls_swap_row{
		3, 3, 3, 3,
		2, 2, 2, 2,
		1, 1, 1, 1,
		4, 4, 4, 4
	};

	constexpr auto mat_swap_rows_02_from_expr{ transposed.swap_rows(0, 2) };
	constexpr auto mat_swap_rows_02_from_mat{ mat_ls_t.swap_rows(0, 2) };

	static_assert(mat_swap_rows_02_from_expr == mat_ls_swap_row);
	static_assert(mat_swap_rows_02_from_mat  == mat_ls_swap_row);

	constexpr static mat_t mat_ls_swap_col{
		3, 2, 1, 4,
		3, 2, 1, 4,
		3, 2, 1, 4,
		3, 2, 1, 4
	};

	//constexpr auto mat_swap_cols_02_from_expr{ mat_ls.swap_cols(0, 2) };
	constexpr auto mat_swap_cols_02_from_mat{ mat_ls.swap_cols(0, 2) };

	//static_assert(mat_swap_cols_02_from_expr == mat_ls_swap_col);
	static_assert(mat_swap_cols_02_from_mat  == mat_ls_swap_col);

	constexpr static mat_t mat_to_invert{
		1, 3, 0, 0,
		0, -1, 4, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	constexpr static mat_t mat_should_be_inverse{
		1, 3, -12, 0,
		0, -1, 4, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	constexpr static auto mat_inverse{ mat_to_invert.inverse() };

	static_assert(mat_inverse == mat_should_be_inverse);

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Iterators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Iterators --\n";

	constexpr auto mat_l_reduced{
		[&](){
			type sum{0};

			for (auto m : mat_ls)
				sum += m;

			return sum;
		}
	};

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	return 0;
}
