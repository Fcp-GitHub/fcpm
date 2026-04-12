#include "math_types/lazy/base.hpp"

struct Mock
{
	//NOTE: these need to be public!
	using store_by_ref = void;	
	using is_lazy      = void;
};

int main()
{
	namespace fml = fcp::math::lazy;

	static_assert(fml::is_heavy_v<Mock> == true);

	static_assert(fml::UseLazyOverload<Mock> == true);
}
