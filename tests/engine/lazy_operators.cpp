#include "core/math_fun.hpp"
#include "linalg/matrix.hpp"

#include "test_utils/test_utils.hpp"

#include <iostream>

template <typename V>
constexpr bool blocks_vv_product{
	!requires(V v0, V v1)
	{
		v0 * v1;
	}
};

int main()
{
	using namespace fcp::math::operators;
	using Double = fcp::math::Scalar<double>;
	using fcp::math::wrap_scalar;

	constexpr Double g{ 9.81 };
	constexpr Double x{ 4 }, vx{ 2 }, t{ 3 };
	//constexpr auto t_sq = t * t;

	constexpr auto boh{ x + t + t };

	//static_assert(t_sq[0] == 9.);

	constexpr auto h{ wrap_scalar(0.5) };

	constexpr auto a{ x + vx*t - 0.5*g*t*t };

	std::cout << "type of a: " << utility::type_name<decltype(a)>() << '\n';

	std::cout << a[0] << '\n';

	static_assert(a[0] != -34.145);
	static_assert(fcp::math::cmp(a[0], -34.145));

	constexpr static fcp::math::RowVector<int, 2> v0{1, 2}, v1{3, 4};
	
	static_assert(blocks_vv_product<fcp::math::RowVector<int, 2>>);
}
