#include "linalg/block_view.hpp"
#include "linalg/matrix.hpp"

#include <iostream>

using type = int;
using mat_t = fcp::math::Matrix<type, 4, 4>;
using const_block_t = fcp::math::BlockView<const mat_t, 2, 2>;
using block_t = fcp::math::BlockView<mat_t, 2, 2>;

int main()
{
	using namespace fcp::math::operators;

	constexpr static mat_t matrix{
		1, 1, 3, 4,
		1, 1, 6, 8,
		1, 6, 7, 7,
		5, 6, 7, 7
	};

	constexpr static const_block_t block{matrix, 0, 0};
	constexpr static auto block_same{ matrix.get_view<2, 2>(0, 0) };

	static_assert(block[0] == 1);

	constexpr auto check{
		[&](){
			for (int i{0}; i < block.size() - 1; i++)
				if (block[i] != block[i+1])
					return false;
			return true;
		}()
	};

	static_assert(check);

	constexpr static const_block_t block1{matrix, 2, 2};	

	static_assert(block1[0] == 7);

	mat_t matrix1{
		1, 1, 3, 4,
		1, 1, 6, 8,
		1, 6, 7, 7,
		5, 6, 7, 7
	};

	std::cout << "Starting matrix: " << '\n' << matrix1 << '\n';

	block_t block_0{matrix1, 0, 0}, block_1{matrix1, 2, 2};

	std::cout << "Starting block at [0, 0]: " << '\n' << block_0 << '\n';
	std::cout << "Starting block at [2, 2]: " << '\n' << block_1 << '\n';

	block_0 = block_1;

	std::cout << "Block at [0, 0] after assignment: " << '\n' << block_0 << '\n';
	std::cout << "Final matrix: " << '\n' << matrix1 << '\n';
	
	for (auto b : block_0)
		std::cout << b << ' ';
	std::cout << std::endl;

	return 0;
}
