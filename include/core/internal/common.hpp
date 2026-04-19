#ifndef FCP_MATH_CORE_COMMON_HPP
#define FCP_MATH_CORE_COMMON_HPP

#include "core/internal/base.hpp"							// IWYU pragma: export
#include "core/internal/forward.hpp"					// IWYU pragma: export
#include "core/internal/hardware.hpp"					// IWYU pragma: export
#include "core/internal/compiler_arch.hpp" 		// IWYU pragma: export

#include "core/constexpr_statements.hpp"		// IWYU pragma: export
#include "core/concepts.hpp"								// IWYU pragma: export
#include "core/traits.hpp"									// IWYU pragma: export

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

template <typename... Vecs>
concept AllSameLayout = 
	(Traits<std::remove_cvref_t<Vecs>>::is_row_major && ...) ||
	(!Traits<std::remove_cvref_t<Vecs>>::is_row_major && ...);

END_FCP_INTERNAL_NAMESPACE

//----------------------------------------------------------------------------------
// Shared features of Matrix classes
//----------------------------------------------------------------------------------

inline constexpr int RowMajor{ 0x01 };
inline constexpr int ColumnMajor{ 0x02 };
inline constexpr int StaticStorage{ 0x04 };
inline constexpr int UseSIMD{ 0x08 };
inline constexpr int JollyFlag{ 0b1111 };	// For scalar types

inline constexpr int MatrixDefaultFlags{ RowMajor | StaticStorage };

START_FCP_INTERNAL_NAMESPACE
template <int Flags>
struct MatrixFlagsInspector
{
	static constexpr bool check_if_set(int flag)
	{
		return (Flags & flag) != 0;
	}
	
	static constexpr bool use_row_major{ check_if_set(RowMajor) };
	static constexpr bool use_static_storage{ check_if_set(StaticStorage) };
	static constexpr bool use_simd{ check_if_set(UseSIMD) };
	
};

template <int LeftFlags, int RightFlags>
struct MatrixFlagsMerger
{
	static constexpr int layout{ (LeftFlags & RowMajor) ? RowMajor : ColumnMajor };
	static constexpr int storage{ StaticStorage };
	static constexpr int simd{ ((LeftFlags & UseSIMD) && (RightFlags & UseSIMD)) ? UseSIMD : 0 };

	static constexpr int value{ layout | storage | simd };

	static constexpr bool use_row_major{ (layout == RowMajor) ? true : false };
};
END_FCP_INTERNAL_NAMESPACE


template <typename T, int N>
using RowVector = Matrix<T, 1, N, RowMajor | StaticStorage>;

template <typename T, int N>
using ColumnVector = Matrix<T, N, 1, ColumnMajor | StaticStorage>;


END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_COMMON_HPP
