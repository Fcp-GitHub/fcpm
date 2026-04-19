#include "benchmark/benchmark.h"
#include "benchmark/utils.h"
#include "linalg/matrix.hpp"

using type = double;
inline constexpr int n{ 50 };
using mat_t = fcp::math::Matrix<type, n, n>;

mat_t A(static_cast<type>(3));
mat_t B(static_cast<type>(5));
mat_t C(static_cast<type>(2));
mat_t D(static_cast<type>(4));	

// Benchmark Matrix constructor
static void constructor(benchmark::State& state)
{
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(mat_t(static_cast<type>(0)));
	}
}
BENCHMARK(constructor);

// Benchmark lazy-evaluated addition
static void lazy_addition_chain(benchmark::State& state)
{
	for (auto _ : state)
	{
		auto result{ (A - B + C - D).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(lazy_addition_chain);

// Benchmark eagerly-evaluated addition
static void eager_addition_chain(benchmark::State& state)
{
	for (auto _ : state)
	{
		auto temp0{ (A - B).eval() };
		auto temp1{ (temp0 + C).eval() };
		auto result{ (temp1 - D).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(eager_addition_chain);

// Benchmark lazy-evalauted scaling
static void lazy_scale(benchmark::State& state)
{
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
	type s{ static_cast<type>(4) };

	for (auto  _ : state)
	{
		auto temp0{ (A * s).eval() };
		auto result{ (temp0 + B).eval() };
		benchmark::DoNotOptimize( result.data() );
	}
}
BENCHMARK(eager_scale);

//Benchmark lazy access after complex operation
static void lazy_access(benchmark::State& state)
{
	type s{ static_cast<type>(4) }, t{ static_cast<type>(7) };

	for (auto _ : state)
	{
		auto result{ (A + B) * s + t * (C + D) };
		benchmark::DoNotOptimize(result[0, n/2]);
		benchmark::DoNotOptimize(result[n/3, 0]);
	}
}
BENCHMARK(lazy_access);

//Benchmark "eager" access after complex operation
static void eager_access(benchmark::State& state)
{
	type s{ static_cast<type>(4) }, t{ static_cast<type>(7) };

	for (auto _ : state)
	{
		auto temp0{ (A + B).eval() };
		auto temp1{ (temp0 * s).eval() };
		auto temp2{ (C + D).eval() };
		auto temp3{ (t * temp2).eval() };
		auto result{ (temp1 + temp2).eval() };
		benchmark::DoNotOptimize(result[0, n/2]);
		benchmark::DoNotOptimize(result[n/3, 0]);
	}
}
BENCHMARK(eager_access);

BENCHMARK_MAIN();
