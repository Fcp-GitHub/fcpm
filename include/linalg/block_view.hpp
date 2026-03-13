#ifndef FCP_MATH_LINALG_BLOCKVIEW_HPP
#define FCP_MATH_LINALG_BLOCKVIEW_HPP

#include "core/base.hpp"
#include "core/interface_base.hpp"

#include <type_traits>
/*#include <ranges>*/

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

START_FCP_INTERNAL_NAMESPACE
template <typename Parent, int NumRows, int NumColumns>
struct Traits<BlockView<Parent, NumRows, NumColumns>>
{
	using ptraits = Traits<std::remove_cvref_t<Parent>>;
	using element_type = ptraits::element_type;
	
	static constexpr int rows{ NumRows };
	static constexpr int columns{ NumColumns };
	static constexpr int size{ rows * columns };	

	static constexpr int flags{ ptraits::flags };

	using materialized_type = Matrix<element_type, rows, columns, flags>;

	static constexpr bool is_row_major{ ptraits::is_row_major };
	static constexpr bool is_writable{ ptraits::is_writable };
};
END_FCP_INTERNAL_NAMESPACE

template <typename Parent, int NumRows, int NumColumns>
class BlockView :
	public internal::InterfaceBase<BlockView<Parent, NumRows, NumColumns>>
	/*,public std::ranges::view_interface<BlockView<Parent, NumRows, NumColumns>>*/
{
	private:
		using ptraits = internal::Traits<std::remove_cvref_t<Parent>>;
		using raw_t = std::remove_cvref_t<Parent>;
		using storage_t = Parent&;

	public:
		constexpr int rows_impl() const { return NumRows; }	
		constexpr int cols_impl() const { return NumColumns; }
		constexpr int flags_impl() const { return m_parent.flags(); }

		constexpr decltype(auto) evaluate(int i) const
		{
			// Map into block/local coordinates
			int local_row{ i / NumColumns };
			int local_col{ i % NumColumns };

			// Delegate the container to do the mapping
			return m_parent.evaluate(local_row + m_pstart_row, local_col + m_pstart_column);	
		}

		constexpr decltype(auto) evaluate(int row, int col) const
		{
			return m_parent.evaluate(
					m_pstart_row + row,
					m_pstart_column + col
			);
		}

		constexpr BlockView() = default;

		constexpr BlockView(Parent&&, int, int) = delete;

		constexpr BlockView(storage_t p, int start_row, int start_col):
			m_parent{std::forward<storage_t>(p)}, 
			m_pstart_row{start_row}, m_pstart_column{start_col} {}

		template <LazyType Expr>
			requires (!std::is_const_v<std::remove_reference_t<Expr>>)
		constexpr BlockView& operator=(Expr expr)
		{
			if constexpr (ptraits::is_row_major)
			{
				for (int row{0}; row < NumRows; row++)
					for (int col{0}; col < NumColumns; col++)
						m_parent[m_pstart_row + row, m_pstart_column + col] = expr[row, col];
			} else {
				for (int col{0}; col < NumColumns; col++)
					for (int row{0}; row < NumRows; row++)
						m_parent[m_pstart_row + row, m_pstart_column + col] = expr[row, col];
			}

			return *this;
		}	

		constexpr BlockView& operator=(const BlockView& other)
		{
			if constexpr (ptraits::is_row_major)
			{
				for (int row{0}; row < NumRows; row++)
					for (int col{0}; col < NumColumns; col++)
						m_parent[m_pstart_row + row, m_pstart_column + col] = other[row, col];
			} else {
				for (int col{0}; col < NumColumns; col++)
					for (int row{0}; row < NumRows; row++)
						m_parent[m_pstart_row + row, m_pstart_column + col] = other[row, col];
			}

			return *this;
		}

	private:
		storage_t m_parent;

		int m_pstart_row, m_pstart_column;
};

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_BLOCKVIEW_HPP
