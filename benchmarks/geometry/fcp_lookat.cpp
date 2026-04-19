#include "linalg/matrix.hpp"
#include "geometry/transforms.hpp"

using type = double;
using vec_t = fcp::math::ColVec3<type>;

int main()
{
	vec_t eye{ 0., 3., 2. }, at{ 0., 0., 0. }, up{ 0., 1., 0. };

	auto result{ fcp::math::look_at(eye, at, up) };
}
