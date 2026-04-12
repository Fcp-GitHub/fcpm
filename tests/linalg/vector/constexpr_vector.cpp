#include "linalg/matrix.hpp"

#include "test_utils/ansi_seq.hpp"

#include <iostream>

using type = int;
using vec_t = fcp::math::RowVector<type, 4>;
using fvec_t = fcp::math::RowVector<double, 4>;
using brd_vec_t = fcp::math::RowVector<type, 3>;

int main()
{
	namespace fcpm = fcp::math;

	//----------------------------------------------------------------------------------
	// Constructors
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Constructors --\n";

	// Notice the static keyword for compile-time evaluation of vectors

	// Default constructor:
	// 1. Default initialization: no, cannot have non-initialized variables
	// 														at compile-time. Furthermore, since compile-time
	// 														programming is based on immutables, I don't
	// 														think it would be even needed
	//constexpr fcpm::Vector<type, 4> vec_d;
	//
	// 2. Value initialization
	constexpr static vec_t vec_v{};

	// List constructor
	constexpr static vec_t vec_l{1, 2, 3, 4};	

	// Broadcast constructor
	constexpr static brd_vec_t vec_to_brd{ 1, 2, 3 };
	constexpr static vec_t vec_brd{ vec_to_brd };

	static_assert(vec_brd == vec_t{1, 2, 3, 0});

	// Copy constructor
	constexpr static vec_t vec_c{ vec_l };

	static_assert(vec_c == vec_l);

	// This is successfully not allowed
	//constexpr static auto identity{ vec_t::identity() };

	// Const vector: not tested, constexpr implies const

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Arithmetic operators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Arithmetic operators --\n";

	// Necessary in any case for ADL
	using namespace fcpm::operators;

	constexpr auto vec_add{ vec_v + vec_l };

	static_assert(vec_add[0] == 1);
	static_assert(vec_add[3] == 4);

	vec_t v = vec_add;
 
	constexpr auto op1{ vec_add + vec_l + vec_add };	// (3, 6, 9, 12)
	constexpr auto op2{ op1 + vec_l + vec_add };			// (5, 10, 15, 20)

	static_assert(op1[2] == 9);
	static_assert(op2[0] == 5);

	constexpr auto vec_sub{ vec_l - vec_c };
	
	static_assert(vec_sub[1] == 0);

	constexpr auto vec_brd_mul{ vec_l * 3 };

	static_assert(vec_brd_mul[0] == 3);

	constexpr auto vec_brd_div{ vec_l / 3 };

	static_assert(vec_brd_div[0] == 0);

	constexpr auto vec_brd_add{ vec_l + 4 };

	static_assert(vec_brd_add[0] == 5);

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Logical operators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Logical operators --\n";
	
	static_assert(vec_l == vec_c);
	static_assert(!(vec_l != vec_c));
	
	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Vector operations
	//----------------------------------------------------------------------------------
	constexpr static fvec_t fvec{ 1., 2., 3., 4. };

	constexpr auto fexpr{ fvec + fvec + fvec };

	constexpr auto cdot{ fcpm::dot(fvec, fvec) };

	static_assert(cdot == 30);

	constexpr auto norm{ fcpm::l2norm_sq(fvec) };

	static_assert(norm == cdot);

	constexpr auto expr_cdot{ fcpm::dot(fexpr, op2) };

	static_assert(expr_cdot == 450);

	constexpr auto expr_norm{ fcpm::l2norm_sq(fexpr) };

	static_assert(expr_norm == 270);

	constexpr auto normalized_v{ fcpm::normalize(vec_l) };

	static_assert(normalized_v[0] == 0);

	constexpr static fcpm::RowVector<type, 3> x{ 1, 0, 0 }, y{ 0, 1, 0 };

	constexpr auto cross_v{ fcpm::cross(x, y) };

	static_assert(cross_v == fcpm::RowVector<type, 3>{ 0, 0, 1 });

	//----------------------------------------------------------------------------------
	// Iterators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Iterators --\n";

	// From `engine/lazy_expression_iterator.cpp`:
	// """
	//  Iterators have to be used differently in a compile-time context vs
	//  a runtime one. Here, I cannot use a range-based loop directly, I must
	//  use a constexpr function and ensure that its result is a constant expression.
	//  Alternatively, I can use a constexpr lambda. In general, the solution lies in
	//  ensuring that the (range-based) loop is in a constexpr context.
	//  In a runtime context I can use range-based loops.
	// """

	constexpr auto vec_l_reduced{
		[&](){
			type sum{0};

			for (auto v : vec_l)
				sum += v;	

			return sum;
		}
	};

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	return 0;
}
