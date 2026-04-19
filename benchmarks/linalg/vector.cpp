#include "benchmark/benchmark.h"
#include "core/common.hpp"
#include "linalg/matrix.hpp"

using type = double;
using vec_t = fcp::math::RowVector<type, 50>;
using small_vec_t = fcp::math::RowVec3<type>;

// Benchmark lazy-evaluated addition
static void lazy_addition_chain(benchmark::State& state)
{
	vec_t a(3.), b(5.), c(2.), d(4.);	

	for (auto _ : state)
	{
		auto result{ (a + b + c + d).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(lazy_addition_chain);

// Benchmark eagerly-evaluated addition
static void eager_addition_chain(benchmark::State& state)
{
	vec_t a(3.), b(5.), c(2.), d(4.);	

	for (auto _ : state)
	{
		auto temp0{ (a + b).eval() };
		auto temp1{ (temp0 + c).eval() };
		auto result{ (temp1 + d).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(eager_addition_chain);

// Benchmark lazy-evaluated cross product
static void lazy_cross_chain(benchmark::State& state)
{
	using fcp::math::cross;

	small_vec_t a(3.), b(5.), c(2.), d(4.), e(6.), f(7.);

	for (auto _ : state)
	{
		auto result{ 
			cross(cross(cross(cross(cross(a, b), c), d), e), f).eval()
		};
		benchmark::DoNotOptimize(result.data());
	}
}
BENCHMARK(lazy_cross_chain);

// Benchmark eagerly-evaluated cross product
static void eager_cross_chain(benchmark::State& state)
{
	using fcp::math::cross;

	small_vec_t a(3.), b(5.), c(2.), d(4.), e(6.), f(7.);

	for (auto _ : state)
	{
		auto temp0{ cross(a, b).eval() };
		auto temp1{ cross(temp0, c).eval() };
		auto temp2{ cross(temp1, d).eval() };
		auto temp3{ cross(temp2, e).eval() };
		auto result{ cross(temp3, f).eval() };
		benchmark::DoNotOptimize(result.data());
	}
}
BENCHMARK(eager_cross_chain);

BENCHMARK_MAIN();
