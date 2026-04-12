#ifndef FCP_MATH_LINALG_INTERNAL_KERNELS_LINSYS_HPP
#define FCP_MATH_LINALG_INTERNAL_KERNELS_LINSYS_HPP

#include "core/common.hpp"
#include "core/math_fun.hpp"

#include "linalg/internal/storage_base.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE
START_FCP_KERNELS_NAMESPACE

//----------------------------------------------------------------------------------
// Back Substitution
//----------------------------------------------------------------------------------

template <LazyMatrixType M, LazyVectorType V, bool IsUnitriangular = false>
constexpr auto backward_substitution_impl(const M& U, const V& b, RowMajorTag)
{
	using T = Traits<std::remove_cvref_t<M>>::element_type;
	constexpr int rows{ Traits<std::remove_cvref_t<M>>::rows };
	constexpr int cols{ Traits<std::remove_cvref_t<M>>::columns };
	constexpr int vsize{ Traits<std::remove_cvref_t<V>>::size };

	std::remove_cvref_t<V> x(static_cast<T>(0));

	for (int i{rows - 1}; i >= 0; i--)
	{
		T sum{ static_cast<T>(0) };

		for (int j{i + 1}; j < cols; j++)
			sum += x[j]*U[i, j];

		if (IsUnitriangular)
			x[i] = b[i] - sum;
		else
			x[i] = (b[i] - sum) / U[i, i];
	}			

	return x;
}

template <LazyMatrixType M, LazyVectorType V, bool IsUnitriangular = false>
constexpr auto backward_substitution_impl(const M& U, const V& b, ColumnMajorTag)
{
	using T = Traits<std::remove_cvref_t<M>>::element_type;
	constexpr int rows{ Traits<std::remove_cvref_t<M>>::rows };
	constexpr int cols{ Traits<std::remove_cvref_t<M>>::columns };
	constexpr int vsize{ Traits<std::remove_cvref_t<V>>::size };

	std::remove_cvref_t<V> x(b);

	for (int j{cols - 1}; j >= 0; j--)
	{
		if (!IsUnitriangular)	x[j] /= U[j, j];

		for (int i{0}; i < j; i++)
		{
			x[i] -= (U[i, j] * x[j]);
		}
	}

	return x;
}

//----------------------------------------------------------------------------------
// Forward Substitution
//----------------------------------------------------------------------------------

template <LazyMatrixType M, LazyVectorType V, bool IsUnitriangular = false>
constexpr auto forward_substitution_impl(const M& L, const V& b, RowMajorTag)
{
	using T = Traits<std::remove_cvref_t<M>>::element_type;
	constexpr int rows{ Traits<std::remove_cvref_t<M>>::rows };
	constexpr int cols{ Traits<std::remove_cvref_t<M>>::columns };
	constexpr int vsize{ Traits<std::remove_cvref_t<V>>::size };

	std::remove_cvref_t<V> y(static_cast<T>(0));

	for (int i{0}; i < rows; i++)
	{
		T sum{ static_cast<T>(0) };

		for (int j{0}; j < i; j++)
			sum += y[j]*L[i, j];

		if constexpr (IsUnitriangular)
			y[i] = b[i] - sum;
		else
			y[i] = (b[i] - sum) / L[i, i];
	}			

	return y;
}

template <LazyMatrixType M, LazyVectorType V, bool IsUnitriangular = false>
constexpr auto forward_substitution_impl(const M& L, const V& b, ColumnMajorTag)
{
	using T = Traits<std::remove_cvref_t<M>>::element_type;
	constexpr int rows{ Traits<std::remove_cvref_t<M>>::rows };
	constexpr int cols{ Traits<std::remove_cvref_t<M>>::columns };
	constexpr int vsize{ Traits<std::remove_cvref_t<V>>::size };

	std::remove_cvref_t<V> y(b);

	for (int j{0}; j < cols; j++)
	{
		if constexpr (!IsUnitriangular) y[j] /= L[j, j];

		for (int i{j + 1}; i < rows; i++)
		{
			y[i] -= (L[i, j] * y[j]);
		}
	}

	return y;
}

//----------------------------------------------------------------------------------
// Gauss Elimination
//----------------------------------------------------------------------------------

template <LazyMatrixType M, LazyVectorType V>
constexpr auto gauss_elimination_impl(M& A, V& b, RowMajorTag)
{
	using T = Traits<std::remove_cvref_t<M>>::element_type;

	constexpr int rank{ Traits<std::remove_cvref_t<M>>::rows };	

	for (int i{0}; i < rank; i++)
	{
		// 1. Partial pivoting
		int pivot_index{ i };
		T max{ fcp::math::abs(A[i, i]) };
		for (int k{i + 1}; k < rank; k++)
			if (fcp::math::abs(A[k, i]) > max)
			{
				max = fcp::math::abs(A[k, i]);
				pivot_index = k;
			}

		//TODO: error handling
		//if (fcp::math::cmp(A[pivot_index, i], static_cast<T>(0)))
		//		return -1;

		// 2. Swap rows (physical swap)
		if (pivot_index != i)
		{
			A.swap_rows(i, pivot_index);
			std::swap(b[i], b[pivot_index]);
		}

		// 3. Elimination
		const T pivot_inv{ 
			static_cast<T>(1) / A[i, i] 
		};

		for (int j{i + 1}; j < rank; j++)
		{
			const T temp{ A[j, i] * pivot_inv };

			A[j, i] = static_cast<T>(0);

			for (int k{i + 1}; k < rank; k++)
				A[j, k] -= temp * A[i, k];

			b[j] -= temp * b[i];
		}
	}

	return backward_substitution_impl(A, b, RowMajorTag{});	
}

template <LazyMatrixType M, LazyVectorType V>
constexpr auto gauss_elimination_impl(M& A, V& b, ColumnMajorTag)
{
	using T = Traits<std::remove_cvref_t<M>>::element_type;

	constexpr int rank{ Traits<std::remove_cvref_t<M>>::rows };

	for (int i{0}; i < rank; i++)
	{
		// 1. Partial pivoting
		//NOTE: contiguous access
		int pivot_index{ i };
		for (int k{i + 1}; k < rank; k++)
			if (fcp::math::abs(A[k, i]) > fcp::math::abs(A[pivot_index, i]))
				pivot_index = k;

		//TODO: error handling

		// 2. Swap rows (physical swap)
		//NOTE: O(n)
		if (pivot_index != i)
		{
			A.swap_rows(i, pivot_index);
			std::swap(b[i], b[pivot_index]);
		}

		// 3. Elimination
		const T pivot_inv{ 
			static_cast<T>(1) / A[i, i] 
		};

		for (int j{i + 1}; j < rank; j++)
		{
			const T temp{ A[j, i] * pivot_inv };

			A[j, i] = static_cast<T>(0);

			for (int k{i + 1}; k < rank; k++)
				A[k, j] -= temp * A[i, k];
		}

		for (int k{i + 1}; k < rank; k++)
			b[k] -= (A[k, i] * pivot_inv) * b[i];
	}

	return backward_substitution_impl(A, b, ColumnMajorTag{});
}

END_FCP_KERNELS_NAMESPACE
END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_INTERNAL_KERNELS_LINSYS_HPP
