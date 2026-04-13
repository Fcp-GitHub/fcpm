#include "benchmark/benchmark.h"
#include "linalg/matrix.hpp"

using type = double;
using mat_t = fcp::math::Matrix<type, 30, 30>;

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


BENCHMARK_MAIN();
