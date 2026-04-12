#include "utility/eager_vector.hpp"

#include <iostream>

using type = float;

/* Tests:
 *  - Constructors:
 *  	- Default
 *  	- List
 *  	- Copy
 *  - constexpr member size_v
 *  - Arithmetic opeators (+, -, *, +(void), -(void), +=, -=)
 *  - Logical operators (==, !=)
 *  - Copy assignment
 *  - Subscript
 *  - Accessor data()
 *	- Mutable iterator
 *	- Constant iterator
 */

template <std::size_t N>
std::ostream& operator<<(std::ostream& out, const fcp::EagerVector<type, N> vec)
{
	constexpr auto dim = vec.size_v;

	out << '(';
	for (std::size_t i{0}; i < dim - 1; i++)
		out << vec[i] << ' '; // subscript operator
	out << vec[dim - 1] << ')'; 

	return out;
}

int main()
{
	//----------------------------------------------------------------------------------
	// Constructors
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Constructors --\n";

	// Default constructor:
	// 1. Default initialization
	fcp::EagerVector<type, 4> vec_d;
	// 2. Value initialization
	fcp::EagerVector<type, 4> vec_v{};

	// List constructor
	fcp::EagerVector<type, 4> vec_l(1.f, 2.f, 3.f, 4.f);	

	// Copy constructor
	fcp::EagerVector<type, 4> vec_c{ vec_l };

	// Const vector
	const fcp::EagerVector<type, 4> const_vec{ vec_l };

	//----------------------------------------------------------------------------------
	// Compile-time variable
	//----------------------------------------------------------------------------------
	auto size = fcp::EagerVector<type, 4>::size_v;

	//----------------------------------------------------------------------------------
	// Arithmetic operators
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Arithmetic operators --\n";

	auto vec_add = vec_l + const_vec;
	auto vec_sub = vec_l - const_vec;
	
	std::cout << "vec_l = " << vec_l << '\n';
	std::cout << "const_vec = " << const_vec << '\n';
	vec_sub      = vec_l + (-const_vec);	// Copy assignment
	std::cout << "vec_sub = vec_l + (-const_vec) = " << vec_sub << '\n';

	auto vec_mul = vec_l * 3;
	vec_add += const_vec;
	vec_sub -= const_vec;

	auto vec_scalar_add = vec_l + 1;
	std::cout << "vec_l + 1 = " << vec_scalar_add << '\n';
	auto vec_scalar_sub = vec_l - 1;
	std::cout << "vec_l - 1 = " << vec_scalar_sub << '\n';

	//----------------------------------------------------------------------------------
	// Logical operators
	//----------------------------------------------------------------------------------

	std::cout << "-- Logical operators --\n";
	
	std::cout << "vec_l = " << vec_l << '\n';
	std::cout << "vec_c = " << vec_c << '\n';
	std::cout << "vec_l == vec_c = " << (vec_l == vec_c) << '\n';
	std::cout << "vec_l != vec_c = " << (vec_l != vec_c) << '\n';

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

	return 0;
}
