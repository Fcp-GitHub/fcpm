#include "core/float_traits.hpp"
#include "core/math_fun.hpp"

#include "test_utils/ansi_seq.hpp"
#include "test_utils/float.hpp"
#include "test_utils/test_utils.hpp"

#include <cmath>
#include <string_view>
#include <string>
#include <array>
#include <ios>
#include <numbers>
#include <iomanip>
#include <iostream>
#include <fstream>

#define QUALIFIER constexpr

namespace internal {
    // Refined coefficients for double precision
		template <typename T>
		constexpr T minimax_sin_core(T x) {
		    T x2 = x * x;
		    return x * (T(1.0L) + x2 * (T(-0.16666666666666666667L) +
		           x2 * (T(0.00833333333333333333L) +
		           x2 * (T(-0.00019841269841269841L)) +
		           x2 * (T(0.00000275573192239859L)))));
		}
		
		template <typename T>
		constexpr T minimax_cos_core(T x) {
		    T x2 = x * x;
		    return T(1.0L) + x2 * (T(-0.49999999999999999999L) +
		           x2 * (T(0.04166666666666666667L) +
		           x2 * (T(-0.00138888888888888889L) +
		           x2 * (T(0.00002480158730158730L)))));
		}
		
		template <typename T>
constexpr T constexpr_sin(T x) {
    // Split PI/2 into High and Low parts for Cody-Waite reduction
    // PI_2_HI has the lower 24 bits of the significand as zero
    const T PI_2_HI = 1.5707963267948965580; 
    const T PI_2_LO = 6.1232339957367658861e-17;
    const T INV_PI_2 = 0.63661977236758134308; // 2/PI

    // 1. Find which quadrant we are in
    // Using a round-to-nearest approach
    long long n = static_cast<long long>(x * INV_PI_2 + (x > 0 ? 0.5 : -0.5));
    
    // 2. Cody-Waite reduction: x* = x - n * pi/2
    T x_star = (x - static_cast<T>(n) * PI_2_HI) - static_cast<T>(n) * PI_2_LO;

    // 3. Selection logic based on quadrant n % 4
    // Quadrant 0: sin(x*), 1: cos(x*), 2: -sin(x*), 3: -cos(x*)
    int quadrant = static_cast<int>(n % 4);
    if (quadrant < 0) quadrant += 4;

    switch (quadrant) {
        case 0: return  minimax_sin_core(x_star);
        case 1: return  minimax_cos_core(x_star);
        case 2: return -minimax_sin_core(x_star);
        case 3: return -minimax_cos_core(x_star);
        default: return 0;
    }
}
}

using type = double;
using ulp_type = fcp::math::internal::FloatTraits<type>::eq_int_t;

using std::numbers::pi_v;
using std::numbers::sqrt2_v;
using std::numbers::sqrt3_v;

constexpr int num_data{ 10 };
constexpr int num_grid{ 11 };

using data_array_type = std::array<type, num_grid>;
using ulp_array_type = std::array<ulp_type, num_grid>;

constexpr data_array_type trig_grid{
	static_cast<type>(0), 
	pi_v<type>/static_cast<type>(6), 
	pi_v<type>/static_cast<type>(4),
	pi_v<type>/static_cast<type>(3), 
	pi_v<type>/static_cast<type>(2), 
	static_cast<type>(2)*pi_v<type>/static_cast<type>(3),
	static_cast<type>(3)*pi_v<type>/static_cast<type>(4), 
	static_cast<type>(5)*pi_v<type>/static_cast<type>(6), 
	pi_v<type>,
	static_cast<type>(3)*pi_v<type>/static_cast<type>(2), 
	static_cast<type>(2)*pi_v<type>
};

constexpr data_array_type cos_expected{
	static_cast<type>(1),
	sqrt3_v<type>/static_cast<type>(2),
	sqrt2_v<type>/static_cast<type>(2),
	static_cast<type>(1)/static_cast<type>(2),
	static_cast<type>(0),
	static_cast<type>(-1)/static_cast<type>(2),
	-sqrt2_v<type>/static_cast<type>(2),
	-sqrt3_v<type>/static_cast<type>(2),
	static_cast<type>(-1),
	static_cast<type>(0),	
	static_cast<type>(1)
};

constexpr data_array_type sin_expected{
	static_cast<type>(0),
	static_cast<type>(1)/static_cast<type>(2),
	sqrt2_v<type>/static_cast<type>(2),
	sqrt3_v<type>/static_cast<type>(2),
	static_cast<type>(1),
	sqrt3_v<type>/static_cast<type>(2),
	sqrt2_v<type>/static_cast<type>(2),
	static_cast<type>(1)/static_cast<type>(2),
	static_cast<type>(0),
	static_cast<type>(-1),
	static_cast<type>(0)	
};


consteval data_array_type test_cos()
{
	data_array_type results;

	for (int i{0}; i < num_grid; i++)
	{
		results[i] = fcp::math::cos(trig_grid[i]);
	}
	
	return results;
}

constexpr data_array_type test_sin()
{
	data_array_type results;

	for (int i{0}; i < num_grid; i++)
	{
		results[i] = internal::constexpr_sin(trig_grid[i]);
	}
	
	return results;
}

ulp_array_type  print_results(
	const data_array_type& results, 
	const data_array_type& expected
)
{
	std::cout << std::left << std::setw(10) << std::setfill(' ') << "Domain";
	std::cout << std::left << std::setw(10) << std::setfill(' ') << "Codomain";
	std::cout << std::left << std::setw(10) << std::setfill(' ') << "Computed";
	std::cout << std::left << std::setw(20) << std::setfill(' ') << "Difference [ULPs]\n";

	ulp_array_type ulps;

	for (int i{0}; i < num_grid; i++)
	{
		ulps[i] = utility::get_ulps(expected[i], results[i]);	
	}

	for (int i{0}; i < num_grid; i++)
	{
		std::cout << std::left << std::setw(10) << std::setfill(' ') << std::showpos << trig_grid[i];
	 	std::cout << std::left << std::setw(10) << std::setfill(' ') <<	std::showpos << expected[i];
		std::cout << std::left << std::setw(10) << std::setfill(' ') <<	std::showpos << results[i];
		std::cout << std::left << std::setw(10) << std::setfill(' ') <<	std::showpos << ulps[i] << '\n';
	}

	return ulps;
}

void export_to_csv(
	std::string_view filename,
	const data_array_type& domain,
	const data_array_type& codomain,
	const data_array_type& results,
	const ulp_array_type& ulps
)
{
	std::ofstream file(filename.data(), std::ios_base::app | std::ios_base::out);

	if (not file.is_open())
	{
		std::cerr << FCPG_FG_RED << "Failed to open file " << filename << FCPG_RESET << '\n'; return;
	}

	// Header
	file << "domain,expected,results,ulp_diff\n";

	// Data
	for (int i{0}; i < num_grid; i++)
	{
		file << domain[i] << ',' << codomain[i] << ',' << results[i] << ',' << ulps[i] << '\n';
	}

	file.close();
	std::cout << FCPG_FG_GREEN << "Data successfully exported to " << filename << FCPG_RESET << '\n';
}

constexpr ulp_type test_cos0()
{
	QUALIFIER auto temp{ fcp::math::cos(static_cast<type>(0)) };
	return utility::get_ulps(temp, static_cast<type>(1));
}

constexpr ulp_type test_sin0()
{
	QUALIFIER auto temp{ fcp::math::sin(static_cast<type>(0)) };
	return utility::get_ulps(temp, static_cast<type>(0));
}

constexpr ulp_type test_cospio4()
{
	constexpr auto two{ static_cast<type>(2) };
	constexpr auto four{ static_cast<type>(4) };
	QUALIFIER auto temp{ fcp::math::cos(std::numbers::pi_v<type>/four) };
	return utility::get_ulps(temp, std::numbers::sqrt2_v<type>/two);
}

constexpr ulp_type test_sinpio4()
{
	constexpr auto two{ static_cast<type>(2) };
	constexpr auto four{ static_cast<type>(4) };
	QUALIFIER auto temp{ fcp::math::sin(std::numbers::pi_v<type>/four) };
	return utility::get_ulps(temp, std::numbers::sqrt2_v<type>/two);
}

constexpr ulp_type test_eq_cossinpio4()
{
	constexpr auto four{ static_cast<type>(4) };
	QUALIFIER auto cos{ fcp::math::cos(std::numbers::pi_v<type>/four) };
	QUALIFIER auto sin{ fcp::math::sin(std::numbers::pi_v<type>/four) };
	return utility::get_ulps(cos, sin);
}

constexpr ulp_type test_cospio2()
{
	constexpr auto two{ static_cast<type>(2) };
	QUALIFIER auto temp{ fcp::math::cos(std::numbers::pi_v<type>/two) };
	return utility::get_ulps(temp, static_cast<type>(0));
}

constexpr ulp_type test_sinpio2()
{
	constexpr auto two{ static_cast<type>(2) };
	QUALIFIER auto temp{ fcp::math::sin(std::numbers::pi_v<type>/two) };
	return utility::get_ulps(temp, static_cast<type>(1));
}

constexpr ulp_type test_cos3pio4()
{
	constexpr auto two{ static_cast<type>(2) };
	constexpr auto three{ static_cast<type>(3) };
	constexpr auto four{ static_cast<type>(4) };
	QUALIFIER auto temp{ fcp::math::cos(std::numbers::pi_v<type>*three/four) };
	return utility::get_ulps(temp, -std::numbers::sqrt2_v<type>/two);
}

constexpr ulp_type test_sin3pio4()
{
	constexpr auto two{ static_cast<type>(2) };
	constexpr auto three{ static_cast<type>(3) };
	constexpr auto four{ static_cast<type>(4) };
	QUALIFIER auto temp{ fcp::math::sin(std::numbers::pi_v<type>*three/four) };
	return utility::get_ulps(temp, std::numbers::sqrt2_v<type>/two);
}

constexpr ulp_type test_cospi()
{
	QUALIFIER auto temp{ fcp::math::cos(std::numbers::pi_v<type>) };
	return utility::get_ulps(temp, static_cast<type>(-1));
}

constexpr ulp_type test_sinpi()
{
	QUALIFIER auto temp{ fcp::math::cos(std::numbers::pi_v<type>) };
	return utility::get_ulps(temp, static_cast<type>(0));
}

constexpr ulp_type test_exp2()
{
	QUALIFIER auto temp{ fcp::math::exp(static_cast<type>(2)) };
	return utility::get_ulps(temp, std::numbers::e_v<type>*std::numbers::e_v<type>);
}

int main()
{
	std::string filename{ "core/results_float/results_double_" };

	std::cout << "Results for type: " << utility::type_name<type>() << '\n';
	std::cout << "Implementation done using Taylor expansion (10 terms)\n";

	std::cout << "\nResults for cosine implementation\n";
	constexpr auto cos_results{ test_cos() };
	const auto cos_ulps{ print_results(cos_results, cos_expected) };

	//export_to_csv(filename + "cos_10.csv", trig_grid, cos_expected, cos_results, cos_ulps);

	std::cout << "\nResults for sine implementation\n";
	const auto sin_results{ test_sin() };
	const auto sin_ulps{ print_results(sin_results, sin_expected) };

	//export_to_csv(filename + "sin_10.csv", trig_grid, sin_expected, sin_results, sin_ulps);

	return 0;
}
