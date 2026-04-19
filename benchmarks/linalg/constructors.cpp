#include "benchmark/benchmark.h"
#include "benchmark/utils.h"
#include "glm/detail/qualifier.hpp"
#include "linalg/matrix.hpp"
#include "glm/glm.hpp"

#include <array>

using type = double;
using mat_t = fcp::math::ColMatrix<type, 4, 4>;

static void fcp_matrix_constructor(benchmark::State& state)
{
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(mat_t(static_cast<type>(0)));
	}
}
BENCHMARK(fcp_matrix_constructor);

static void fcp_matrix_def_init(benchmark::State& state)
{
	for (auto _ : state)
	{
		mat_t result;
		benchmark::DoNotOptimize(result[0, 0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[0, 1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[0, 2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[0, 3] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1, 0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1, 1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1, 2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1, 3] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2, 0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2, 1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2, 2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2, 3] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3, 0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3, 1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3, 2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3, 3] = static_cast<type>(0));
	}
}
BENCHMARK(fcp_matrix_def_init);

static void glm_matrix_def_init(benchmark::State& state)
{
	for (auto _ : state)
	{
		glm::mat<4, 4, type> result;
		benchmark::DoNotOptimize(result[0][0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[0][1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[0][2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[0][3] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1][0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1][1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1][2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[1][3] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2][0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2][1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2][2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[2][3] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3][0] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3][1] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3][2] = static_cast<type>(0));
		benchmark::DoNotOptimize(result[3][3] = static_cast<type>(0));
	
	}
}
BENCHMARK(glm_matrix_def_init);

static void std_fill(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::array<type, 16> result;
		std::fill(result.begin(), result.end(), static_cast<type>(0));
		benchmark::DoNotOptimize(result.data());
	}
}
BENCHMARK(std_fill);

BENCHMARK_MAIN();
