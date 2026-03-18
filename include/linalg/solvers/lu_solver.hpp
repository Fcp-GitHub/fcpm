#ifndef FCP_MATH_LINALG_SOLVERS_LU_SOLVER_HPP
#define FCP_MATH_LINALG_SOLVERS_LU_SOLVER_HPP

#include "core/common.hpp"
#include "core/math_fun.hpp"

#include "linalg/internal/storage_base.hpp"
#include "linalg/solvers/dense_solvers.hpp"

#include <type_traits>
#include <array>
#include <numeric>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE
START_FCP_SOLVERS_NAMESPACE

template <LazyMatrixLike Matrix>
class LUSolver
{
	using traits = internal::Traits<std::remove_cvref_t<Matrix>>;
	using T      = traits::element_type;
	using mat_t  = traits::materialized_type;

	public:
		constexpr LUSolver(const Matrix& matrix):
			m_buffer{std::forward<mat_t>(matrix.eval())}
		{
			using layout = std::conditional_t<
				traits::is_row_major,
				RowMajorTag,
				ColumnMajorTag
			>;				

			std::iota(m_pivi.begin(), m_pivi.end(), 0);

			factorize(layout{});
		}

		template <LazyVectorLike Vector>
		constexpr auto solve(const Vector& b) const
		{
			using vtraits = internal::Traits<std::remove_cvref_t<Vector>>;

			// Apply permutations to b
			Vector pb;
			for (int i{0}; i < vtraits::size; i++)
				pb[i] = b[m_pivi[i]];

			return backward_substitution(
					m_buffer, 
					forward_substitution<true>(m_buffer, pb)
			);		
		}

		constexpr auto get_determinant() const
		{
			constexpr int rank = traits::rows;
			double det{ 1 };

			for (int i{0}; i < rank; i++)
			{
				det *= m_buffer[i, i];
			}
			
			return (m_is_perm_even ? det : (-det));
		}

		constexpr auto get_inverse() const
		{
			constexpr int rank = traits::rows;
			std::remove_cvref_t<Matrix> result;

			// System to resolve: AX = 1_n
			for (int i{0}; i < rank; i++)
			{
				// 1. Create i-th column of the identity matrix
				ColumnVector<T, traits::rows> e_i(static_cast<T>(0));
				e_i[i] = static_cast<T>(1);

				// 2. Apply permutation to e_i
				ColumnVector<T, traits::rows> perm;
				for (int j{0}; j < rank; j++)
					perm[j] = e_i[m_pivi[j]];

				// 3. Solve Ly = e_i via forward substitution
				auto y{ forward_substitution<true>(m_buffer, perm) };

				// 4. Solve Ux = y via backward substitution
				auto x{ backward_substitution(m_buffer, y) };

				// 5. Store the result
				for (int row{0}; row < rank; row++)
					result[row, i] = x[row];
			}

			return result;
		}

	private:
		constexpr void factorize(RowMajorTag)
		{
			constexpr int rank{ traits::rows };

			for (int k{0}; k < rank; k++)
			{
				// Partial pivoting
				int pivot_row_index{ k };
				T max{ internal::abs(m_buffer[k, k]) };
				for (int i{k + 1}; i < rank; i++)
				{
					T temp{ internal::abs(m_buffer[i, k]) };

					if (temp > max)
					{
						max = temp;
						pivot_row_index = i;
					}
				}

				if (pivot_row_index != k)
				{
					m_buffer.swap_rows(k, pivot_row_index);
					std::swap(m_pivi[k], m_pivi[pivot_row_index]);
					m_is_perm_even = !m_is_perm_even;
				}

				T pivot_inv{ static_cast<T>(1) / m_buffer[k, k] };

				for (int i{k + 1}; i < rank; i++)
				{
					m_buffer[i, k] *= pivot_inv;

					T temp{ m_buffer[i, k] };

					for (int j{k + 1}; j < rank; j++)
						m_buffer[i, j] -= temp * m_buffer[k, j];
				}
			}
		}

		constexpr void factorize(ColumnMajorTag)
		{
			constexpr int rank{ traits::rows };

			for (int k{0}; k < rank; k++)
			{
				// Partial pivoting
				int pivot_row_index{ k };
				T max{ internal::abs(m_buffer[k, k]) };
				for (int i{k + 1}; i < rank; i++)
				{
					T temp{ internal::abs(m_buffer[i, k]) };

					if (temp > max)
					{
						max = temp;
						pivot_row_index = i;
					}
				}

				if (pivot_row_index != k)
				{
					m_buffer.swap_rows(k, pivot_row_index);
					std::swap(m_pivi[k], m_pivi[pivot_row_index]);
				}

				T pivot_inv{ static_cast<T>(1) / m_buffer[k, k] };

				for (int i{k + 1}; i < rank; i++)
					m_buffer[i, k] *= pivot_inv;

				for (int j{k + 1}; j < rank; j++)
				{
					T temp{ m_buffer[k, j] };

					for (int i{k + 1}; i < rank; i++)
						m_buffer[i, j] -= m_buffer[i, k] * temp;
				}
			}
		}

		bool m_is_perm_even{ true };
		std::array<T, traits::rows> m_pivi;
		mat_t m_buffer;
};

END_FCP_SOLVERS_NAMESPACE
END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE

#endif	//FCP_MATH_LINALG_SOLVERS_LU_SOLVER_HPP
