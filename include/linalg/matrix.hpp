#ifndef FCP_MATH_LINALG_MATRIX_HPP
#define FCP_MATH_LINALG_MATRIX_HPP

#include "core/common.hpp"
#include "linalg/internal/storage_base.hpp"
#include "core/interface_base.hpp"

#include <type_traits>
#include <algorithm>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE
template <typename T, int NumRows, int NumColumns, int Flags>
struct Traits<Matrix<T, NumRows, NumColumns, Flags>>
{
		using element_type = T;
		using materialized_type = Matrix<T, NumRows, NumColumns, Flags>;

		static constexpr int rows{ NumRows };
		static constexpr int columns{ NumColumns };
		static constexpr int size{ rows * columns };

		static constexpr int flags{ Flags };

		static constexpr bool is_row_major{ MatrixFlagsInspector<Flags>::use_row_major };
		static constexpr bool is_writable{ true };
};

template <typename T, int NumRows, int NumColumns, int Flags>
struct MatrixStorageInspector
{
	using LayoutType = std::conditional_t<
		MatrixFlagsInspector<Flags>::use_row_major,
		RowMajorTag,
		ColumnMajorTag
	>;

	using StorageBaseType = std::conditional_t<
 		MatrixFlagsInspector<Flags>::use_static_storage,
		StaticStorageBase<T, NumRows, NumColumns, LayoutType>,
		void
	>;
};
END_FCP_INTERNAL_NAMESPACE

template <
	typename T, int NumRows, int NumColumns, 
	int Flags = RowMajor | StaticStorage
>
class Matrix : 
	public internal::MatrixStorageInspector<T, NumRows, NumColumns, Flags>::StorageBaseType,
	public internal::InterfaceBase<Matrix<T, NumRows, NumColumns, Flags>>
{
	using storage_inspector = internal::MatrixStorageInspector<T, NumRows, NumColumns, Flags>;
	using Layout = typename storage_inspector::LayoutType;
	using StorageBase = typename storage_inspector::StorageBaseType;
	using StorageBase::m_data;

	public:
		using is_matrix = void;
		using StorageBase::StorageBase;

		constexpr int rows_impl() const { return NumRows; }
		constexpr int cols_impl() const { return NumColumns; }
		constexpr int flags_impl() const { return Flags; }

		constexpr const T& evaluate(int i) const
		{
			return m_data[i];	
		}

		constexpr T& evaluate(int i)
		{
			return m_data[i];	
		}

		constexpr const T& evaluate(int row, int col) const
		{
			return m_data[compute_index(row, col)];	
		}

		constexpr T& evaluate(int row, int col)
		{
			return m_data[compute_index(row, col)];	
		}

		// Redefine explicitly the default constructor
		// otherwise ADL doesn't find it
		constexpr Matrix() = default;

		// Broadcasting constructor
		constexpr Matrix(T value) { std::fill(this->begin(), this->end(), value); }

		// Vector list constructor
		template <LazyVectorLike... Vecs>
			requires (sizeof...(Vecs) > 0) //&& internal::AllSameLayout<Vecs...>
		constexpr Matrix(Vecs&&... vecs)	
		{
			// First make sure the number of vectors is correct
			constexpr int total_input_size{ (internal::Traits<std::remove_cvref_t<Vecs>>::size + ...) };

			static_assert(total_input_size <= NumRows*NumColumns);

			int vector_index{ 0 };

			auto assign_ith_vector{
				[&](auto&& vec) {
					for (int i{ 0 }; i < vec.size(); i++)
					{
						using raw_t = std::remove_cvref_t<decltype(vec)>;	

						// If vectors are RowVectors...
						if constexpr (internal::Traits<raw_t>::is_row_major)
						{
							this->m_data[compute_index(vector_index, i)] = vec[i];
						} else { // ...else they are ColumnVectors
							this->m_data[compute_index(i, vector_index)] = vec[i];
						}
					}

					vector_index++;
				}
			};

			// Iterate through all the vectors via pack expansion
			(assign_ith_vector(std::forward<Vecs>(vecs)), ...);

			// Zero-initialize all the remaining elements
			for (int i{total_input_size}; i < (NumRows*NumColumns); i++)
				m_data[i] = 0;
		}

		// Lazy constructor
		constexpr Matrix(LazyExpressionType auto expr)
		{
			lazy_assign(expr, Layout{});
		}

		// Lazy assignment operator
		constexpr Matrix& operator=(LazyType auto expr)
		{
			lazy_assign(expr, Layout{});
			return *this;
		}

	using iterator = internal::ExpressionIterator<Matrix>;

	private:
		constexpr void lazy_assign(auto expr, auto tag){}

		constexpr void lazy_assign(auto expr, RowMajorTag)
		{
			if constexpr (NumColumns == 1)
			{
				for (int i{0}; i < NumRows; i++)
					m_data[i] = expr.evaluate(i);
			}
			else {
				for (int row{0}; row < NumRows; row++)
					for (int col{0}; col < NumColumns; col++)
						m_data[compute_index(row, col)] = expr.evaluate(row, col);
			}
		}
		
		constexpr void lazy_assign(auto expr, ColumnMajorTag)
		{
			if constexpr (NumRows == 1)
			{
				for (int i{0}; i < NumColumns; i++)
					m_data[i] = expr.evaluate(i);
			} else {
				for (int col{0}; col < NumColumns; col++)
					for (int row{0}; row < NumRows; row++)
						m_data[compute_index(row, col)] = expr.evaluate(row, col);
			}
		}

		constexpr int compute_index(int row, int col) const
		{
			if constexpr (std::is_same_v<Layout, RowMajorTag>)
			{
				if constexpr (NumColumns == 1) return row;
				else return row*NumColumns + col;
			}
			else
			{
				if constexpr (NumRows == 1) return col;
				else return col*NumRows + row;
			}
		}
};

template <typename T, int N>
using RowVector = Matrix<T, 1, N, RowMajor | StaticStorage>;

template <typename T, int N>
using ColumnVector = Matrix<T, N, 1, ColumnMajor | StaticStorage>;

END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	// FCP_MATH_LINALG_MATRIX_HPP
