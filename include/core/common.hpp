#ifndef FCP_MATH_CORE_COMMON_HPP
#define FCP_MATH_CORE_COMMON_HPP

#include "core/base.hpp"
#include "core/forward.hpp"

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

template <typename T>
inline constexpr bool is_writable_v{
			Traits<std::remove_cvref_t<T>>::is_writable &&
			/*std::is_lvalue_reference_v<T> &&*/
			(!std::is_const_v<T>)	
};

template <typename... Vecs>
concept AllSameLayout = 
	(Traits<std::remove_cvref_t<Vecs>>::is_row_major && ...) ||
	(!Traits<std::remove_cvref_t<Vecs>>::is_row_major && ...);

END_FCP_INTERNAL_NAMESPACE

inline constexpr int RowMajor{ 0x01 };
inline constexpr int ColumnMajor{ 0x02 };
inline constexpr int StaticStorage{ 0x04 };
inline constexpr int UseSIMD{ 0x08 };

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
};

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_CORE_COMMON_HPP
