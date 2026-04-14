#include "benchmark/benchmark.h"
#include "linalg/matrix.hpp"
#include "linalg/solvers/lu_solver.hpp"

using type = double;
using mat_t = fcp::math::Matrix<type, 20, 20>;
using small_mat_t = fcp::math::Matrix<type, 2, 2>;

// Benchmark lazy-evaluated addition
static void lazy_addition_chain(benchmark::State& state)
{
	mat_t A(3.), B(5.), C(2.), D(4.);	

	for (auto _ : state)
	{
		auto result{ (A + B + C + D).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(lazy_addition_chain);

// Benchmark eagerly-evaluated addition
static void eager_addition_chain(benchmark::State& state)
{
	mat_t A(3.), B(5.), C(2.), D(4.);	

	for (auto _ : state)
	{
		auto temp0{ (A + B).eval() };
		auto temp1{ (temp0 + C).eval() };
		auto result{ (temp1 + D).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(eager_addition_chain);

// Benchmark lazy-evalauted scaling
static void lazy_scale(benchmark::State& state)
{
	mat_t A(3.), B(5.);
	type s{ static_cast<type>(4) };

	for (auto  _ : state)
	{
		auto result{ (A * s + B).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(lazy_scale);

// Benchmark eagerly-evalauted scaling
static void eager_scale(benchmark::State& state)
{
	mat_t A(3.), B(5.);
	type s{ static_cast<type>(4) };

	for (auto  _ : state)
	{
		auto temp0{ (A * s).eval() };
		auto result{ (temp0 + B).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(eager_scale);

// Benchmark GEMM in two different sequence of operations
static void gemm_first_scale(benchmark::State& state)
{
	mat_t A(3.), B(5.);
	type s{ static_cast<type>(4) };

	for (auto _ : state)
	{
		benchmark::DoNotOptimize( ((s * A) * B).eval().data() );
	}
}
BENCHMARK(gemm_first_scale);

static void gemm_first_multiply(benchmark::State& state)
{
	mat_t A(3.), B(5.);
	type s{ static_cast<type>(4) };

	for (auto _ : state)
	{
		benchmark::DoNotOptimize( ((A * B) * s).eval().data() );
	}
}
BENCHMARK(gemm_first_scale);

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


BENCHMARK_MAIN();
