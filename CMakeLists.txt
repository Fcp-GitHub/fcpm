cmake_minimum_required(VERSION 3.6)
project(
	fcpm
	VERSION 0.5
	LANGUAGES CXX
)
message(STATUS "FCPM: version 0.5")

option(FCPM_BUILD_TESTS "Build the test programs" OFF)
option(FCPM_BUILD_BENCHMARKS "Build the benchmark suite (Google Benchmark) and programs" OFF)
option(FCPM_USE_COMPILER_ATTRIBUTES "Use compiler-specific attributes" ON)

if (FCPM_USE_COMPILER_ATTRIBUTES)
	add_compile_definitions(FCPM_USE_COMPILER_ATTRIBUTES=true)
endif()

include(GNUInstallDirs)

if (FCPM_BUILD_TESTS)
	#include(CTest)
	add_subdirectory(tests)
endif()
