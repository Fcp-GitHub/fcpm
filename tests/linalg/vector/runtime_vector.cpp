#include "linalg/matrix.hpp"

#include "test_utils/ansi_seq.hpp"

#include <iostream>
#include <random>
#include <algorithm>
#include <complex>

#define PRINT_RESULT(OP, EXP) 				 \
	std::cout << #OP " = " << OP << " (" \
	<< (OP == EXP ? \
			FCPG_FG_GREEN "correct" FCPG_RESET :\
		  FCPG_FG_RED  	"wrong" FCPG_RESET \
		 ) \
	<< ")\n";

using type = int;
using vec_t = fcp::math::RowVector<type, 4>;

int main()
{
	//----------------------------------------------------------------------------------
	// Constructors
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Constructors --\n";

	// Default constructor:
	// 1. Default initialization
	vec_t vec_d;
	// 2. Value initialization (probably zero initialization)
	vec_t vec_v{};

	// List constructor
	vec_t vec_l{1, 2, 3, 4};

	// Copy constructor
	vec_t vec_c{ vec_l };

	// Const vector
	const vec_t const_vec{ vec_l };

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Arithmetic operators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Arithmetic operators --\n";

	// Necessary in any case for ADL
	using namespace fcp::math::operators;

	auto vec_add{ vec_v + vec_l };

	PRINT_RESULT(vec_add[0], 1);
	
	auto op1{ vec_add + vec_l + vec_add };
	auto op2{ op1 + vec_l + vec_add };

	PRINT_RESULT(op1[2], 9);
	PRINT_RESULT(op2[0], 5);

	auto vec_sub{ vec_l - vec_c };

	PRINT_RESULT(vec_sub[0], 0);

	auto vec_brd_mul{ vec_l * 3 };

	PRINT_RESULT(vec_brd_mul[0], 3);

	auto vec_brd_add{ vec_l + 4 };

	PRINT_RESULT(vec_brd_add[0], 5);

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Logical operators
	//----------------------------------------------------------------------------------

	std::cout << "-- Logical operators --\n";
	
	std::cout << "vec_l = " << vec_l << '\n';
	std::cout << "vec_c = " << vec_c << '\n';

	PRINT_RESULT((vec_l == vec_c), true);
	PRINT_RESULT((vec_l != vec_c), false);

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Iterators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Iterators --\n";

	for (auto v : vec_l)
		std::cout << v << ' ';
	std::cout << std::endl;

	for (auto v : const_vec)
		std::cout << v << ' ';
	std::cout << std::endl;

	std::iota(vec_v.begin(), vec_v.end(), 0);

	PRINT_RESULT(vec_v[2], 2);

	std::random_device rnd;
	std::mt19937 gen{rnd()};	

	std::shuffle(vec_v.begin(), vec_v.end(), gen);

	std::cout << vec_v << '\n';

	using namespace std::complex_literals;

	std::complex<type> a{ 1, 3 };
	std::complex<type> b{ 2, 2 };
	fcp::math::RowVector<std::complex<type>, 2> complex_vec{ a, b };

	std::cout << complex_vec << '\n';

	PRINT_RESULT(complex_vec[0].imag(), 3);

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	return 0;
}
