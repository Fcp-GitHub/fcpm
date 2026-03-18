#ifndef FCP_MATH_LINALG_STORAGE_HPP
#define FCP_MATH_LINALG_STORAGE_HPP

#include "core/common.hpp"

#include <array>
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

// Tags
//TODO: should they be defined here?
struct RowMajorTag {};
struct ColumnMajorTag {};


START_FCP_INTERNAL_NAMESPACE

// Static storage class
template <typename T, int N, int M, typename Layout>
struct StaticStorageBase
{
	// Default constructor
	constexpr StaticStorageBase() = default;

	// Instantiate from a list of scalars
	// If Layout is ColumnMajor the matrix is transposed in order
	// to maintain readability while allowing for column-major storage
	constexpr StaticStorageBase(ScalarType auto&&... args):
		m_data{std::forward<decltype(args)>(args)...}
	{
		static_assert(sizeof...(args) == N*M, "The number of elements is not N*M");	

		if constexpr (std::is_same_v<Layout, ColumnMajorTag> and ((N != 1) && (M != 1)))
		{
			for (int i{0}; i < N; i++)
				for (int j{i}; j < M; j++)
				{
					T temp{ m_data[i*M + j] };	
					m_data[i*M + j] = m_data[j*M + i];
					m_data[j*M + i] = temp;
				}
		}
	}

	// Instantiate from an array
	// Data is cloned with no further manipulation
	constexpr StaticStorageBase(std::array<T, N*M> data):
		m_data{data} {}

	// Copy constructor
	constexpr StaticStorageBase(const StaticStorageBase& other)
	{
		m_data = other.m_data;	
	}

	// Copy assignment opeator
	constexpr StaticStorageBase operator=(const StaticStorageBase& other)
	{
		m_data = other.m_data;
	}

	// Get the underlying buffer
	constexpr auto data() const { return m_data.data(); }

	std::array<T, N*M> m_data;
};

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_STORAGE_HPP 
