#ifndef FCP_MATH_GEOMETRY_QUATERNION_CLASS_HPP
#define FCP_MATH_GEOMETRY_QUATERNION_CLASS_HPP

#include "core/internal/common.hpp"

#include "linalg/internal/matrix_class.hpp"

#include "geometry/internal/quaternion_base.hpp"
#include "geometry/internal/quaternion_operations.hpp"

START_FCP_NAMESPACE
START_FCP_MATH_NAMESPACE

//----------------------------------------------------------------------------------
// Generic Quaternion Class
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE

// Quaternion traits
template <typename T>
struct Traits<Quaternion<T>>
{
	using vtraits = Traits<ColumnVector<T, 4>>;

	using plugin_t = Traits<QuaternionBase<Quaternion<T>>>::plugin_t;

	using element_type = T;
	using materialized_type = Quaternion<T>;
	
	static constexpr int rows{ 4 };
	static constexpr int columns{ 1 };
	static constexpr int size{ 4 };

	static constexpr int flags{ vtraits::flags };

	static constexpr bool is_row_major{ vtraits::is_row_major };
	static constexpr bool is_writable{ true };
};

END_FCP_INTERNAL_NAMESPACE

template <typename T>
class Quaternion : public internal::QuaternionBase<Quaternion<T>>
{
	using base = internal::QuaternionBase<Quaternion<T>>;
	
	public:
		using base::base;	// Inherit constructors
										
		constexpr auto inverse() const
		{
			using fcp::math::operators::operator*;
			const auto normsq_inv{ static_cast<T>(1) / l2norm_sq(*this) };	
			return Quaternion<T>( (this->conjugate() * normsq_inv).eval() );
		}
};

//----------------------------------------------------------------------------------
// Unit Quaternion Class
//----------------------------------------------------------------------------------

START_FCP_INTERNAL_NAMESPACE

// Quaternion traits
template <typename T>
struct Traits<UnitQuaternion<T>>
{
	using vtraits = Traits<ColumnVector<T, 4>>;

	using plugin_t = Traits<QuaternionBase<UnitQuaternion<T>>>::plugin_t;

	using element_type = T;
	using materialized_type = UnitQuaternion<T>;
	
	static constexpr int rows{ 4 };
	static constexpr int columns{ 1 };
	static constexpr int size{ 4 };

	static constexpr int flags{ vtraits::flags };

	static constexpr bool is_row_major{ vtraits::is_row_major };
	static constexpr bool is_writable{ true };
};

END_FCP_INTERNAL_NAMESPACE

template <typename T>
class UnitQuaternion : public internal::QuaternionBase<UnitQuaternion<T>>
{
	using base = internal::QuaternionBase<UnitQuaternion<T>>;

	public:
		using base::base;	// Inherit constructors
		using is_unit_quaternion = void;

		constexpr auto inverse() const
		{
			return this->conjugate();
		}

		constexpr UnitQuaternion<T>& renormalize()
		{
			using operators::operator/=;

			const auto norm{ l2norm(*this) };

			if (!fcp::math::cmp(norm, static_cast<T>(0)))
				*this /= norm;

			return *this;
		}
};


END_FCP_MATH_NAMESPACE
END_FCP_NAMESPACE


#endif	//FCP_MATH_GEOMETRY_QUATERNION_CLASS_HPP
