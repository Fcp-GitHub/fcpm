#include "benchmark/benchmark.h"
#include "linalg/matrix.hpp"
#include "linalg/solvers/lu_solver.hpp"

inline constexpr int n{ 50 };
using type = double;
using mat_t = fcp::math::RowMatrix<type, n, n>;
using small_mat_t = fcp::math::Matrix<type, 4, 4>;
using vec_t = fcp::math::ColumnVector<type, n>;

static void lu_linsys(benchmark::State& state)
{
	mat_t A(3.);
	vec_t b(2.);

	for (auto _ : state)
	{
		fcp::math::solvers::LUSolver<mat_t> solver(A);
		benchmark::DoNotOptimize(solver.solve(b).eval());
	}
}
BENCHMARK(lu_linsys);

static void optimized_small_inverse(benchmark::State& state)
{
	small_mat_t A(3.);

	for (auto _ : state)
	{
		benchmark::DoNotOptimize(A.inverse().eval().data());
	}
}
BENCHMARK(optimized_small_inverse);

static void bruteforce_small_inverse(benchmark::State& state)
{
	small_mat_t A(3.);

	for (auto _ : state)
	{
		fcp::math::solvers::LUSolver<small_mat_t> solver(A);	
		benchmark::DoNotOptimize(solver.get_inverse().eval().data());
	}
}
BENCHMARK(bruteforce_small_inverse);

static void bruteforce_small_determinant(benchmark::State& state)
{
	small_mat_t A(3.);

	for (auto _ : state)
	{
		fcp::math::solvers::LUSolver<small_mat_t> solver(A);	
		benchmark::DoNotOptimize(solver.get_determinant());
	}
}
BENCHMARK(bruteforce_small_determinant);

static void std_determinant(benchmark::State& state)
{
	mat_t A(3.);

	for (auto _ : state)
	{
		fcp::math::solvers::LUSolver<mat_t> solver(A);
		benchmark::DoNotOptimize(solver.get_determinant());
	}
}
BENCHMARK(std_determinant);


BENCHMARK_MAIN();
