#ifndef FCP_MATH_GEOMETRY_INTERNAL_QUATERNION_BASE_HPP
#define FCP_MATH_GEOMETRY_INTERNAL_QUATERNION_BASE_HPP

#include "core/common.hpp"
#include "core/interface_base.hpp"
#include "core/expression_iterator.hpp"
#include "core/math_fun.hpp"

#include "linalg/matrix.hpp"

#include <numbers>
#include <type_traits>

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Quaternion Base Class
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE

// Need traits for InterfaceBase
template <typename Derived>
struct Traits<QuaternionBase<Derived>>
{
	using dtraits = Traits<std::remove_cvref_t<Derived>>;

	using plugin_t = QuaternionPlugin<QuaternionBase<Derived>>;
	
	using element_type = dtraits::element_type;
	using materialized_type = dtraits::materialized_type;

	static constexpr int rows{ dtraits::rows };
	static constexpr int columns{ dtraits::columns };
	static constexpr int size{ dtraits::size };

	static constexpr int flags{ dtraits::flags };

	static constexpr bool is_row_major{ dtraits::is_row_major };
	static constexpr bool is_writable{ dtraits::is_writable };
};

// Implement here all common functionalities
// between materialized quaternions and expression templates
template <typename Derived>
struct QuaternionPlugin
{
	using traits = Traits<std::remove_cvref_t<Derived>>;
	using T = traits::element_type;

	// Get scalar component
	template <typename Self>
	constexpr decltype(auto) scalar(this Self&& self)
	{ 
		return self.evaluate(0); 
	}

	// Get vector component
	template <typename Self>
	constexpr decltype(auto) vector(this Self&& self)
	{
		return ColumnVector<T, 3>{
				self.evaluate(1), self.evaluate(2), self.evaluate(3)
		};
	}
};

template <typename Derived>
class QuaternionBase : public InterfaceBase<QuaternionBase<Derived>>
{
	using traits = Traits<std::remove_cvref_t<Derived>>;
	using T = traits::element_type;

	public:
		using is_quaternion = void;
		using iterator = ExpressionIterator<QuaternionBase<Derived>>;


		// Constructors
		//NOTE: default constructor is re-defined here for ADL
		//NOTE: define any specialized constructor before the delegating one for ADL

		// Default constructor
		constexpr QuaternionBase() = default;

		// Construct a quaternion specifying its scalar and vector parts
		constexpr QuaternionBase(T scalar, const ColumnVector<T, 3>& vector):
			m_data{scalar, vector[0], vector[1], vector[2]} {}

		// Construct a quaternion specifying its scalar and vector parts
		constexpr QuaternionBase(T scalar, ColumnVector<T, 3>&& vector):
			m_data{scalar, vector[0], vector[1], vector[2]} {}

		// Generic quaternion constructor
		template <typename... Args>
		constexpr QuaternionBase(Args&&... args):
			m_data{std::forward<Args>(args)...} {}	// Leverage ColumnVector's constructors

		// Lazy constructor
		//TODO: no perfect forwarding?
		template <LazyExpressionType E>
		constexpr QuaternionBase(E expr):
			m_data(std::forward<E>(expr)) {}

		// Lazy assignment
		template <LazyQuaternionLike L>
		constexpr QuaternionBase& operator=(L expr)
		{
			m_data = expr.m_data;
			return *this;
		}

		// Returns an identity quaternion
		static constexpr Derived identity()
		{
			return Derived(static_cast<T>(1), ColumnVector<T, 3>(static_cast<T>(0)));
		}	

		// Create a quaternion from a given matrix
		template <bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT>
		static constexpr Derived from_matrix(const LazyMatrixType auto& matrix)
		{
			// Helper enumeration for the switch statement
			enum {W, X, Y, Z} max_index{ W };

			// Start from w (init value for max)
			auto max{ matrix[0, 0] + matrix[1, 1] + matrix[2, 2] };	

			// Compute x
			auto temp{ matrix[0, 0] - matrix[1, 1] - matrix[2, 2] };
			if (max < temp) { max = temp; max_index = X; }

			// Compute y
			temp = matrix[1, 1] - matrix[0, 0] - matrix[2, 2];
			if (max < temp) { max = temp; max_index = Y; }

			// Compute z
			temp = matrix[2, 2] - matrix[0, 0] - matrix[1, 1];	
			if (max < temp) { max = temp; max_index = Z; }

			// Complete formula for the chosen element
			max = fcp::math::sqrt(max + static_cast<T>(1)) / static_cast<T>(2);

			// 2. Compute the other 3 elements
			const auto qmax_inv{ static_cast<T>(0.25L) / max };

			if constexpr (ColumnVectorFormat)
			{
				switch(max_index)
				{
					case W: return Derived{
										max,
										(matrix[2, 1] - matrix[1, 2]) * qmax_inv,
										(matrix[0, 2] - matrix[2, 0]) * qmax_inv,
										(matrix[1, 0] - matrix[0, 1]) * qmax_inv
									};

					case X: return Derived{
										(matrix[2, 1] - matrix[1, 2]) * qmax_inv,
										max,
										(matrix[1, 0] + matrix[0, 1]) * qmax_inv,
										(matrix[0, 2] + matrix[2, 0]) * qmax_inv
									};

					case Y: return Derived{
										(matrix[0, 2] - matrix[2, 0]) * qmax_inv,
										(matrix[1, 0] + matrix[0, 1]) * qmax_inv,
										max,
										(matrix[2, 1] + matrix[1, 2]) * qmax_inv
									};

					// case Z
					default: return Derived{
										(matrix[1, 0] - matrix[0, 1]) * qmax_inv,
										(matrix[0, 2] + matrix[2, 0]) * qmax_inv,
										(matrix[2, 1] + matrix[1, 2]) * qmax_inv,
										max
									};
				}
			} else {
				switch(max_index)
				{
					case W: return Derived{
										max,
										(matrix[1, 2] - matrix[2, 1]) * qmax_inv,
										(matrix[2, 0] - matrix[0, 2]) * qmax_inv,
										(matrix[0, 1] - matrix[1, 0]) * qmax_inv
									};

					case X: return Derived{
										(matrix[1, 2] - matrix[2, 1]) * qmax_inv,
										max,
										(matrix[0, 1] + matrix[1, 0]) * qmax_inv,
										(matrix[2, 0] + matrix[0, 2]) * qmax_inv
									};

					case Y: return Derived{
										(matrix[2, 0] - matrix[0, 2]) * qmax_inv,
										(matrix[0, 1] + matrix[1, 0]) * qmax_inv,
										max,
										(matrix[1, 2] + matrix[2, 1]) * qmax_inv
									};

					// case Z
					default: return Derived{
										(matrix[0, 1] - matrix[1, 0]) * qmax_inv,
										(matrix[2, 0] + matrix[0, 2]) * qmax_inv,
										(matrix[1, 2] + matrix[2, 1]) * qmax_inv,
										max
									};
				}
			}
		}

		// Create a quaternion from a given set of Euler angles
		// CONVENTIONS:
		// - Euler angles order: heading-pitch-bank (roll-pitch-yaw)
		// - Quaternion rotation: object space to upright space
		template <typename T>
		constexpr static Derived from_euler(T heading, T pitch, T bank)
		{
			auto cos_h{ fcp::math::cos(heading/2) };
			auto cos_p{ fcp::math::cos(pitch/2) };
			auto cos_b{ fcp::math::cos(bank/2) };

			auto sin_h{ fcp::math::sin(heading/2) };
			auto sin_p{ fcp::math::sin(pitch/2) };
			auto sin_b{ fcp::math::sin(bank/2) };

			return Derived{	
				cos_h*cos_p*cos_b + sin_h*sin_p*sin_b,
				cos_h*sin_p*cos_b + sin_h*cos_p*sin_b,
				sin_h*cos_p*cos_b - cos_h*sin_p*sin_b,
				cos_h*cos_p*sin_b - sin_h*sin_p*cos_b
			};
		}

		// Create a quaternion from a given set of Euler angles
		// CONVENTIONS:
		// - Euler angles order: heading-pitch-bank (roll-pitch-yaw)
		// - Quaternion rotation: object space to upright space
		template <typename T>
		constexpr static Derived from_euler(std::array<T, 3> angles)
		{
			return from_euler(angles[0], angles[1], angles[2]);
		}
	
		template <typename Self>
		constexpr decltype(auto) evaluate(this Self&& self, int i)
		{
			return self.m_data[i];
		}	

		constexpr Derived& operator=(LazyType auto expr)
		{
			m_data = expr;
			return *this;
		}

		//template <typename Self>
		//constexpr decltype(auto) evaluate(this Self&& self, int i, int j)
		//{
		//	return self.m_data[i, j];
		//}	

		template <typename Self>
		constexpr decltype(auto) data(this Self&& self)
		{
			return self.m_data.data();
		}

		// No transpose for quaternions
		constexpr auto transpose() const = delete;

		constexpr auto conjugate() const
		{
			return ConjugateExpr<Derived, T>(derived());
		}

		// Convert quaternion to specified matrix type
		//NOTE: defaults to column-major matrix
		//NOTE: defaults to column-vector format
		template <
			bool ColumnVectorFormat = FCPM_GRAPHICS_USE_CVECTOR_FORMAT, 
			LazyMatrixType Matrix = ColMatrix<T, 4, 4>
		>
		constexpr auto to_matrix() const
		{
			using mtraits = Traits<Matrix>;

			// Utility
			const auto& w{ m_data[0] };
			const auto& x{ m_data[1] };
			const auto& y{ m_data[2] };
			const auto& z{ m_data[3] };

			constexpr auto zero{ static_cast<T>(0) };

			if constexpr (ColumnVectorFormat)
			{
				return Matrix{
					1 - 2*y*y - 2*z*z, 2*x*y - 2*w*z, 2*x*z + 2*w*y, zero,
					2*x*y + 2*w*z, 1 - 2*x*x - 2*z*z, 2*y*z + 2*w*x, zero,
					2*x*z - 2*w*y, 2*y*z - 2*w*x, 1 - 2*x*x - 2*y*y, zero,
					zero, zero, zero, static_cast<T>(1)
				};
			} else {
				return Matrix{
					1 - 2*y*y - 2*z*z, 2*x*y + 2*w*z, 2*x*z - 2*w*y, zero,
					2*x*y - 2*w*z, 1 - 2*x*x - 2*z*z, 2*y*z + 2*w*x, zero,
					2*x*z + 2*w*y, 2*y*z - 2*w*x, 1 - 2*x*x - 2*y*y, zero,
					zero, zero, zero, static_cast<T>(1)
				};
			}
		}

		// Convert quaternion to a set of Euler angles
		// CONVENTIONS:
		// - Euler angles order: [heading, pitch, bank]
		// - Quaternion rotation: from object space to upright space
		constexpr std::array<T, 3> to_euler() const
		{
			enum { H, P, B } index{ H };		
			std::array<T, 3> result;
			constexpr auto pio2{ std::numbers::pi_v<T> / static_cast<T>(2) };

			// Utility
			const auto& w{ m_data[0] };
			const auto& x{ m_data[1] };
			const auto& y{ m_data[2] };
			const auto& z{ m_data[3] };

			// Get sin(pitch)
			const auto sinp{ static_cast<T>(-2) * (y*z - w*x) };

			// Check for Gymbal Lock
			if (fcp::math::abs(sinp) > static_cast<T>(0.9999))
			{
				// Gymbal Lock (object "looking" straight up or down)
				result[P] = pio2 * sinp;	
				result[H] = fcp::math::atan2(-x*z + w*y, static_cast<T>(0.5) - y*y - z*z);
				result[B] = static_cast<T>(0);
			} else {
				result[P] = fcp::math::asin(sinp);
				result[H] = fcp::math::atan2(x*z + w*y, static_cast<T>(0.5) - x*x - y*y);
				result[B] = fcp::math::atan2(x*y + w*z, static_cast<T>(0.5) - x*x - z*z);
			}			

			return result;
		}

	private:

		//CONVENTION: [s; x, y, z]
		ColumnVector<T, 4> m_data;

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

#endif	//FCP_MATH_GEOMETRY_INTERNAL_QUATERNION_BASE_HPP
