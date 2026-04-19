#ifndef FCPUT_LAZYEVAL_EXPRESSION_ITERATOR_HPP
#define FCPUT_LAZYEVAL_EXPRESSION_ITERATOR_HPP

#include "core/internal/common.hpp"

#include <cstddef>
#include <iterator>
#include <type_traits>
//#include <ranges>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_INTERNAL_NAMESPACE

// View used to iterate through specific rows/columns
template <typename Iterator>
struct IteratorRange //: std::ranges::view_interface<IteratorRange<Iterator>>
{
	Iterator m_first, m_last;

	constexpr Iterator begin() const { return m_first; }
	constexpr Iterator end()   const { return m_last;  }
};

template <typename T>
struct iterator_storage
{
	using raw_t = std::remove_cvref_t<T>;

	// C++ doesn't have "const reference" types, thus
	// apply const to the underlying type before turning
	// it into a reference
	using base_t = std::conditional_t<
		std::is_const_v<std::remove_reference_t<T>>,
		const raw_t,
		raw_t
	>;

	using type = std::conditional_t<
		is_writable_v<raw_t>, 
		base_t&, 
		raw_t	// Keep as non-const for move-safety
	>;
};

// Iterator for the expression template classes
template </*LazyType*/ typename Expr, int Stride = 1>
class ExpressionIterator
{
	using traits = Traits<std::remove_cvref_t<Expr>>;

	public:
	
		// Member typedefs	
		using value_type        = traits::element_type;

		// Use references only if the right overload allows it
		using reference         = decltype(std::declval<Expr>()[0]);	

		using difference_type   = std::ptrdiff_t;
		using pointer           = value_type*;

		using iterator_category = std::conditional_t<
			std::is_lvalue_reference_v<reference>,
			std::random_access_iterator_tag,
			std::input_iterator_tag
		>;

		// Constructors

		// Default constructor
		constexpr ExpressionIterator(): 
			m_expr{}, m_index{0} {}

		// Constructor
		constexpr ExpressionIterator(auto&& expr, int i):
			m_expr{std::forward<decltype(expr)>(expr)}, m_index{i} {}

		// Copy constructor
		constexpr ExpressionIterator(const ExpressionIterator& other):
			m_expr{other.m_expr}, m_index{other.m_index} {}

		// Operators

		// Dereferencing and subscripting triggers lazy evaluation
		FCPM_ALWAYS_INLINE constexpr reference operator*() const { return m_expr[m_index]; }
		FCPM_ALWAYS_INLINE constexpr reference operator[](difference_type n) const { return m_expr[m_index + n*Stride]; }

		constexpr ExpressionIterator& operator++() { m_index += Stride; return *this; }
		constexpr ExpressionIterator& operator++(int) { m_index += Stride; return *this; }
		constexpr ExpressionIterator& operator--() { m_index -= Stride; return *this; }
		constexpr ExpressionIterator& operator--(int) { m_index -= Stride; return *this; }

		//constexpr ExpressionIterator& operator+=(difference_type n) { m_index += n * Stride; return *this; }
		//constexpr ExpressionIterator& operator-=(difference_type n) { m_index -= n * Stride; return *this; }

		template <typename Self>
		constexpr ExpressionIterator& operator+=(this Self&& self, difference_type n) 
		{ 
			self.m_index += n * Stride; 
			return self; 
		}
		
		template <typename Self>
		constexpr ExpressionIterator& operator-=(this Self&& self, difference_type n) 
		{ 
			self.m_index -= n * Stride; 
			return self; 
		}
		
		template <typename Self>
		constexpr ExpressionIterator operator+(this Self&& self, difference_type n)
		{
			ExpressionIterator temp(self);
			return temp += n;
		}

		template <typename Self>
		constexpr ExpressionIterator operator-(this Self&& self, difference_type n)
		{
			ExpressionIterator temp(self);
			return temp -= n;
		}

		FCPM_ALWAYS_INLINE FCPM_CONST
		constexpr difference_type operator-(ExpressionIterator& other) const
		{
			return static_cast<difference_type>(m_index) - static_cast<difference_type>(other.m_index);
		}

		// TODO: this can't be right
		FCPM_ALWAYS_INLINE FCPM_CONST
		constexpr bool operator==(const ExpressionIterator& other) const
		{
			return /*(m_expr == other.m_expr) and*/ (m_index == other.m_index);
		}

		constexpr auto operator<=>(const ExpressionIterator& other) const = default;

		friend constexpr void swap(ExpressionIterator& it1, ExpressionIterator& it2)
		{
			using std::swap;
			//swap(it1.m_expr, it2.m_expr);	//TODO
			swap(it1.m_index, it2.m_index);
		}

	private:
		typename iterator_storage<Expr>::type m_expr;
		int m_index;
};

END_FCP_INTERNAL_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCPUT_LAZYEVAL_EXPRESSION_ITERATOR_HPP
