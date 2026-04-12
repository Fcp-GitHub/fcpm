#ifndef FCP_MATH_T_DETAIL_EAGER_VEC_HPP
#define FCP_MATH_T_DETAIL_EAGER_VEC_HPP

#include "math_types/common.hpp"

#include <array>

START_FCP_NAMESPACE

template <typename T, std::size_t N>
class EagerVector
{
	public:
		
		//----------------------------------------------------------------------------------
		// Compile-Time Properties
		//----------------------------------------------------------------------------------
		constexpr static std::size_t size_v = N;

		//----------------------------------------------------------------------------------
		// Constructors
		//----------------------------------------------------------------------------------

		/// @brief Default constructor
		constexpr EagerVector() = default;

		/// @brief Create a vector from a list of scalar values
		template <typename ...Args>
		constexpr EagerVector(const Args&... args): m_data{args...} 
		{
			static_assert(sizeof...(args) == N, "Number of arguments must be equal to N.");
		}
		
		/// @brief Copy constructor
		constexpr EagerVector(const EagerVector<T, N>& other)
		{
			this->m_data = other.m_data;
		}

		/// @brief Copy assignment
		constexpr EagerVector<T, N>& operator=(const EagerVector<T, N>& other)
		{
			m_data = other.m_data;
			return *this;
		}

		//----------------------------------------------------------------------------------
		// Arithmetic Operators
		//----------------------------------------------------------------------------------

		/// @brief Vector-Vector sum
		constexpr EagerVector<T, N> operator+(const EagerVector<T, N>& other) const
		{
			EagerVector<T, N> result;

			for (std::size_t i{0}; i < N; i++)
				result.m_data[i] = m_data[i] + other.m_data[i];

			return result;
		}

		// Vector-scalar sum (short for vector-broadcast sum)
		constexpr EagerVector<T, N> operator+(const T value) const
		{
			EagerVector<T, N> result;

			for (std::size_t i{0}; i < N; i++)
				result.m_data[i] = m_data[i] + value;

			return result;
		}
		
		/// @brief Vector-Vector subtraction
		constexpr EagerVector<T, N> operator-(const EagerVector<T, N>& other) const
		{
			EagerVector<T, N> result;

			for (std::size_t i{0}; i < N; i++)
				result.m_data[i] = m_data[i] - other.m_data[i];
			
			return result;
		}
		
		// Vector-scalar subtraction (short for vector-broadcast subtraction)
		constexpr EagerVector<T, N> operator-(const T value) const
		{
			EagerVector<T, N> result;

			for (std::size_t i{0}; i < N; i++)
				result.m_data[i] = m_data[i] - value;

			return result;
		}

		/// @brief Vector-Scalar multiplication
		constexpr EagerVector<T, N> operator*(const T& scalar) const
		{
			EagerVector<T, N> result;
			
			for (std::size_t i{0}; i < N; i++)
				result.m_data[i] = m_data[i] * scalar;

			return result;
		}

		/// @brief Multiply vector by +1 (does nothing)
		constexpr EagerVector<T, N>& operator+(void) const
		{
			return *this;
		}

		/// @brief Multiply vector by -1
		constexpr EagerVector<T, N> operator-(void) const
		{
			return (*this) * static_cast<T>(-1);	
		}

		/// @brief Vector-Vector fused add-assign
		constexpr EagerVector<T, N>& operator+=(const EagerVector<T, N>& other)
		{
			*this = *this + other;
			return *this;
		}

		/// @brief Vector-Vector fused subtract-assign
		constexpr EagerVector<T, N>& operator-=(const EagerVector<T, N>& other)
		{
			*this = *this - other;
			return *this;
		}

		//----------------------------------------------------------------------------------
		// Logic operators
		//----------------------------------------------------------------------------------

		constexpr bool operator==(const EagerVector<T, N>& other)
		{
			for (std::size_t i{ 0 }; i < N; i++)
				if (m_data[i] != other.m_data[i])
					return false;

			return true;
		}
			
		constexpr bool operator!=(const EagerVector<T, N>& other)
		{
			return !(*this == other);
		}

		//----------------------------------------------------------------------------------
		// Access operators	
		//----------------------------------------------------------------------------------
		constexpr T operator[](const std::size_t i) const
		{
			return m_data[i];
		}

		T& operator[](const std::size_t i)
		{
			return m_data[i];
		}

		//----------------------------------------------------------------------------------
		// Accessors
		//----------------------------------------------------------------------------------
		constexpr const T* data(void) const
		{
			return m_data.data();
		}
		
		//----------------------------------------------------------------------------------
		// Iterator interface
		//----------------------------------------------------------------------------------

		using iter  = typename std::array<T, N>::iterator;
		using citer = typename std::array<T, N>::const_iterator;

		constexpr iter begin(void) { return m_data.begin(); }
		constexpr iter end(void) { return m_data.end(); }
		constexpr citer begin(void) const { return m_data.cbegin(); }
		constexpr citer end(void) const { return m_data.cend(); }
		constexpr citer cbegin(void) const { return m_data.cbegin(); }
		constexpr citer cend(void) const { return m_data.cend(); }

	private:
		std::array<T, N> m_data;
};

END_FCP_NAMESPACE

#endif	//FCP_MATH_T_DETAIL_EAGER_VEC_HPP
