#ifndef FCP_MATH_LINALG_SOLVERS_GEMM_HPP
#define FCP_MATH_LINALG_SOLVERS_GEMM_HPP

#include "core/common.hpp"
#include "core/hardware.hpp"

#include "linalg/internal/storage_base.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE
START_FCP_KERNELS_NAMESPACE

//----------------------------------------------------------------------------------
// Tiny matrix solver
//----------------------------------------------------------------------------------

// Tiny matrix solver
// Every loop and operation is unrolled at compile-time, rendering it
// perfect for tiny matrices (2x2 - 4x4)
template <int Index, int Common>
constexpr auto unrolled_dot(const auto& left, const auto& right, int row, int col)
{
	if constexpr ((Index + 1) < Common)
	{
		return (left[row, Index] * right[Index, col]) + 
			unrolled_dot<Index + 1, Common>(left, right, row, col);
	} else {
		return left[row, Index] * right[Index, col];
	}
}

template <int LeftRows, int RightColumns, int Common>
constexpr void gemm_tiny(auto& res, const auto& left, const auto& right)
{
	for_constexpr<0, LeftRows>([&](auto i) {
			for_constexpr<0, RightColumns>([&](auto j) {
					res[i, j] = unrolled_dot<0, Common>(left, right, i, j);
			});
	});
}

//----------------------------------------------------------------------------------
// Standard matrix solver (small to medium size)
//----------------------------------------------------------------------------------

// Standard matrix solver
// Maximizes spatial locality and is thus perfect for matrices
// that fit inside the L1 cache
template <int LeftRows, int RightColumns, int Common>
constexpr void gemm_standard(
		auto& res, const auto& left, const auto& right, 
		auto left_tag, auto right_tag
){}

template <int LeftRows, int RightColumns, int Common>
constexpr void gemm_standard(auto& res, const auto& left, const auto& right, RowMajorTag, RowMajorTag)
{
	for (int i{0}; i < LeftRows; i++)
		for (int k{0}; k < Common; k++)	
		{
			auto temp{ left[i, k] };

			for (int j{0}; j < RightColumns; j++)
				res[i, j] += temp * right[k, j];
		}
}

template <int LeftRows, int RightColumns, int Common>
constexpr void gemm_standard(auto& res, const auto& left, const auto& right, ColumnMajorTag, ColumnMajorTag)
{
	for (int j{0}; j < RightColumns; j++)
		for (int k{0}; k < Common; k++)
		{
			auto temp{ right[k, j] };
			for (int i{0}; i < LeftRows; i++)
				res[i, j] += left[i, k] * temp;
		}
}

template <int LeftRows, int RightColumns, int Common>
constexpr void gemm_standard(auto& res, const auto& left, const auto& right, RowMajorTag, ColumnMajorTag)
{
	for (int i{0}; i < LeftRows; i++)
		for (int j{0}; j < RightColumns; j++)
		{
			auto temp{ left[i, j] };
			
			for (int k{0}; k < Common; k++)
				res[i, j] += temp * right[k, j];
		}
}

template <int LeftRows, int RightColumns, int Common>
constexpr void gemm_standard(auto& res, const auto& left, const auto& right, ColumnMajorTag, RowMajorTag)
{
	for (int k{0}; k < Common; k++)
		for (int j{0}; j < RightColumns; j++)
		{
			auto temp{ right[k, j] };
			
			for (int i{0}; i < LeftRows; i++)
				res[i, j] += left[i, k] * temp;
		}
}

//----------------------------------------------------------------------------------
// GEMM routine dispatcher
//----------------------------------------------------------------------------------

// Gemm routine dispatcher
template <int MergedFlags, typename Result, typename LeftExpr, typename RightExpr>
constexpr void gemm_dispatcher(Result& result, const LeftExpr& lexpr, const RightExpr& rexpr)
{
	using ltraits = Traits<std::remove_cvref_t<LeftExpr>>;
	using rtraits = Traits<std::remove_cvref_t<RightExpr>>;
	using inspector = MatrixFlagsInspector<MergedFlags>;
	using element_t = std::common_type_t<
		typename ltraits::element_type, 
		typename rtraits::element_type
	>;

	using llayout = std::conditional_t<
		ltraits::is_row_major,
		RowMajorTag,
		ColumnMajorTag
	>;

	using rlayout = std::conditional_t<
		rtraits::is_row_major,
		RowMajorTag,
		ColumnMajorTag
	>;

	constexpr int lrows{ ltraits::rows };
	constexpr int rcols{ rtraits::columns };
	constexpr int common{ ltraits::columns };

	constexpr bool left_is_square{ common == lrows };
	constexpr bool right_is_square{ rcols == common };

	constexpr int total_bytes{ 
		(lrows*common + common*rcols + lrows*rcols) * sizeof(element_t)
 	};

	if constexpr (lrows <= 4 && rcols <= 4 && common <= 4)
		gemm_tiny<lrows, rcols, common>(result, lexpr, rexpr);
	else if constexpr (total_bytes < FCP_MATH_L1_SIZE)
		gemm_standard<lrows, rcols, common>(result, lexpr, rexpr, llayout{}, rlayout{});
}

END_FCP_KERNELS_NAMESPACE
END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_SOLVERS_GEMM_HPP
