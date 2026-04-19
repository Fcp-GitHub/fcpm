#ifndef FCP_MATH_LINALG_SOLVERS_DENSE_SOLVERS_HPP
#define FCP_MATH_LINALG_SOLVERS_DENSE_SOLVERS_HPP

#include "core/internal/common.hpp"
#include "linalg/internal/kernels/linsys.hpp"

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_SOLVERS_NAMESPACE

template <bool IsUnitriangular = false, LazyMatrixType M, LazyVectorType V>
constexpr auto backward_substitution(const M& U, const V& b)
{
	using layout = std::conditional_t<
		internal::Traits<std::remove_cvref_t<M>>::is_row_major,
		RowMajorTag,
		ColumnMajorTag
	>;

	return internal::kernels::backward_substitution_impl<M, V, IsUnitriangular>(
			U, b, layout{}
	);
}

template <bool IsUnitriangular = false, LazyMatrixType M, LazyVectorType V>
constexpr auto forward_substitution(const M& L, const V& b)
{
	using layout = std::conditional_t<
		internal::Traits<std::remove_cvref_t<M>>::is_row_major,
		RowMajorTag,
		ColumnMajorTag
	>;

	return internal::kernels::forward_substitution_impl<M, V, IsUnitriangular>(
			L, b, layout{}
	);
}

template <LazyMatrixType M, LazyVectorType V>
constexpr auto gauss_elimination(M& A, V& b)
{
	using mtraits = internal::Traits<std::remove_cvref_t<M>>;
	
	static_assert(mtraits::rows == mtraits::columns, "A has to be a square matrix.");

	using layout = std::conditional_t<
		mtraits::is_row_major,
		RowMajorTag,
		ColumnMajorTag
	>;

	return internal::kernels::gauss_elimination_impl(A, b, layout{});
}

END_FCP_SOLVERS_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_SOLVERS_DENSE_SOLVERS_HPP
