#ifndef FCP_MATH_LINALG_INTERFACE_BASE_HPP
#define FCP_MATH_LINALG_INTERFACE_BASE_HPP

#include "core/forward.hpp"
#include "core/expression_iterator.hpp"
#include "core/math_fun.hpp"

#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

template <typename Derived>
struct InterfaceBase
{
	using is_lazy = void;
	using dtraits  = Traits<std::remove_cvref_t<Derived>>;
	using element_type = dtraits::element_type;

	constexpr int rows() const { return dtraits::rows; }	
	constexpr int columns() const { return dtraits::columns; }	
	constexpr int flags() const { return dtraits::flags; }
	constexpr int size() const { return dtraits::size; }

	constexpr auto operator[](int row, int col) const 
	{ 
		return derived().evaluate(row, col); 
	}

	constexpr decltype(auto) operator[](int row, int col) requires is_writable_v<Derived>
	{
		return derived().evaluate(row, col);
	}

	constexpr auto operator[](int i) const
	{
		return derived().evaluate(i);
	}

	constexpr decltype(auto) operator[](int i) requires is_writable_v<Derived>
	{
		return derived().evaluate(i);
	}

	constexpr auto at(int row, int col) const
	{
		if (row < rows() and col < columns()) 
			return derived().evaluate(row, col);
	}

	constexpr decltype(auto) at(int row, int col)
	{
		if (row < rows() and col < columns()) 
			return derived().evaluate(row, col);
	}

	constexpr auto at(int i) const
	{
		if (i < size())
			return derived().evaluate(i);
	}

	constexpr decltype(auto) at(int i)
	{
		if (i < size())
			return derived().evaluate(i);
	}

	constexpr auto front() const
	{
		return derived().evaluate(0, 0);
	}

	constexpr auto back() const
	{
		return derived().evaluate(rows()-1, columns()-1);
	}

	friend constexpr bool operator==(const Derived& first, const LazyType auto& second)
	{
		static_assert(
				Traits<std::remove_cvref_t<Derived>>::size == 
				Traits<std::remove_cvref_t<decltype(second)>>::size,
				"The two operands should have the same size."			
		);

		for (int i{0}; i < first.size(); i++)
			if (!cmp(first[i], second[i]))
				return false;

		return true;
	}

	constexpr auto eval() const
	{
		using raw_t    = std::remove_cvref_t<Derived>;
		using result_t = typename Traits<raw_t>::materialized_type;

		result_t result{};

		if constexpr (requires { derived().materialize_to(result); })
		{
			derived().materialize_to(result);	
		} else {
			for (int i{ 0 }; i < result.size(); i++)
				result[i] = derived()[i];	
		}
		
		return result;
	}

	template <typename Self>
	constexpr auto swap_rows(this Self&& self, int i, int j)
	{
		if constexpr (is_writable_v<Self>)
		{
			self.swap_rows_impl(i, j);
			return self;
		} else {
			return PermutationExpr<Self>::add_row_swap(
					std::forward<Self>(self), 
					i, j
			);	
		}
	}
	
	template <typename Self>
	constexpr auto swap_cols(this Self&& self, int i, int j)
	{
		if constexpr (is_writable_v<Self>)
		{
			self.swap_cols_impl(i, j);
			return self;
		} else {
			return PermutationExpr<Self>::add_col_swap(
					std::forward<Self>(self), 
					i, j
			);	
		}
	}

	constexpr auto transpose() const 
	{
		return TransposeExpr<
			Derived, 
			element_type
		>(derived());		
	}

	constexpr auto block() const;	//TODO
															
	constexpr auto inverse() const
	{
		return InverseExpr<
			Derived,
			element_type
		>(derived());
	}
	
	//----------------------------------------------------------------------------------
	// Contiguous iterator interface
	//----------------------------------------------------------------------------------

	// I/O iterator: for matrices that are lvalues
	template <typename Self>
	constexpr auto begin(this Self& self) noexcept 
		requires is_writable_v<Derived>
	{ 
		return ExpressionIterator<Self&>(self, 0);
	}

	template <typename Self>
	constexpr auto end(this Self& self) noexcept 
		requires is_writable_v<Derived>
	{ 
		return ExpressionIterator<Self&>(self, self.size());
	}

	// input iterator: for const matrices and expressions
	template <typename Self>
	constexpr auto begin(this const Self& self) noexcept
	{
		return ExpressionIterator<const Self&>(self, 0);
	}

	template <typename Self>
	constexpr auto end(this const Self& self) noexcept 
	{ 
		return ExpressionIterator<const Self&>(self, self.size());
	}

	// input iterator: for temporaries
	template <typename Self>
	constexpr auto begin(this Self&& self) noexcept
	{
		return ExpressionIterator<std::remove_cvref_t<Self>>(std::move(self), 0);
	}

	template <typename Self>
	constexpr auto end(this Self&& self) noexcept 
	{ 
		return ExpressionIterator<std::remove_cvref_t<Self>>(std::move(self), self.size());
	}

	constexpr auto cbegin() const noexcept 
	{ 
		return ExpressionIterator<Derived>(derived(), 0);
	}

	constexpr auto cend() const noexcept 
	{ 
		return ExpressionIterator<Derived>(derived(), derived().size());
	}

	//----------------------------------------------------------------------------------
	// Column-iterator interface
	//----------------------------------------------------------------------------------

	// Get a range view of i-th column
	template <typename Self>
	constexpr auto column(this Self&& self, int i)
	{
		return internal::IteratorRange{
			self.col_begin(i), self.col_end(i)
		};
	}

	template <typename Self>
	constexpr auto col_begin(this Self&& self, int column_index) noexcept
	{
		using raw_t = std::remove_cvref_t<Derived>;
		using traits = Traits<raw_t>;

		constexpr int offset{ traits::is_row_major ? 1 : traits::rows };
		constexpr int stride{ traits::is_row_major ? traits::columns : 1 };

		using copy_const_t = std::conditional_t<
			std::is_const_v<std::remove_reference_t<Self>>,
			std::add_const_t<raw_t>,
			raw_t
		>;

		using argument_t = std::conditional_t<
			std::is_lvalue_reference_v<Self>,
			std::add_lvalue_reference_t<copy_const_t>,
			std::remove_cvref_t<raw_t>
		>;

		return ExpressionIterator<argument_t, stride>{
			static_cast<argument_t>(self), column_index*offset
		};
	}

	template <typename Self>
	constexpr auto col_end(this Self&& self, int column_index) noexcept
	{
		using raw_t = std::remove_cvref_t<Derived>;
		using traits = Traits<raw_t>;

		constexpr int offset{ traits::is_row_major ? 1 : traits::rows };
		constexpr int stride{ traits::is_row_major ? traits::columns : 1 };

		using copy_const_t = std::conditional_t<
			std::is_const_v<std::remove_reference_t<Self>>,
			std::add_const_t<raw_t>,
			raw_t
		>;

		using argument_t = std::conditional_t<
			std::is_lvalue_reference_v<Self>,
			std::add_lvalue_reference_t<copy_const_t>,
			std::remove_cvref_t<raw_t>
		>;
				
		return ExpressionIterator<argument_t, stride>{
			static_cast<argument_t>(self), (column_index*offset) + (traits::rows*stride)
		};
	}

	//----------------------------------------------------------------------------------
	// Row-iterator interface
	//----------------------------------------------------------------------------------

	// Get a range view of i-th column
	template <typename Self>
	constexpr auto row(this Self&& self, int i)
	{
		return internal::IteratorRange{
			self.row_begin(i), self.row_end(i)
	};
	}
	
	template <typename Self>
	constexpr auto row_begin(this Self&& self, int row_index) noexcept
	{
		using raw_t = std::remove_cvref_t<Derived>;
		using traits = Traits<raw_t>;

		constexpr int offset{ traits::is_row_major ? traits::columns : 1 };
		constexpr int stride{ traits::is_row_major ? 1 : traits::rows };

		using copy_const_t = std::conditional_t<
			std::is_const_v<std::remove_reference_t<Self>>,
			std::add_const_t<raw_t>,
			raw_t
		>;

		using argument_t = std::conditional_t<
			std::is_lvalue_reference_v<Self>,
			std::add_lvalue_reference_t<copy_const_t>,
			std::remove_cvref_t<raw_t>
		>;
				
		return ExpressionIterator<argument_t, stride>{
			static_cast<argument_t>(self), (row_index*offset)
		};
	}

	template <typename Self>
	constexpr auto row_end(this Self&& self, int row_index) noexcept
	{
		using raw_t = std::remove_cvref_t<Derived>;
		using traits = Traits<raw_t>;

		constexpr int offset{ traits::is_row_major ? traits::columns : 1 };
		constexpr int stride{ traits::is_row_major ? 1 : traits::rows };

		using temp_t = std::conditional_t<
			std::is_const_v<std::remove_reference_t<Self>>,
			std::add_lvalue_reference_t<std::add_const_t<raw_t>>,
			std::add_lvalue_reference_t<raw_t>
		>;

		using argument_t = std::conditional_t<
			std::is_lvalue_reference_v<Self>,
			temp_t,
			std::remove_cvref_t<raw_t>
		>;
				
		return ExpressionIterator<argument_t, stride>{
			static_cast<argument_t>(self), (row_index*offset) + (traits::columns*stride)
		};
	}

	private:
		constexpr Derived& derived() noexcept
		{
			return static_cast<Derived&>(*this);
		}

		constexpr const Derived& derived() const noexcept
		{
			return static_cast<const Derived&>(*this);
		}
};

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_INTERFACE_BASE_HPP
