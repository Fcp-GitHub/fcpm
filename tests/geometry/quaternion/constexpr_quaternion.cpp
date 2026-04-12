#include "core/math_fun.hpp"
#include "geometry/internal/quaternion_operations.hpp"
#include "linalg/matrix.hpp"
#include "geometry/quaternion.hpp"

#include "test_utils/ansi_seq.hpp"

#include <iostream>
#include <numbers>

using type = float;
using quat_t = fcp::math::Quaternion<type>;
using uquat_t = fcp::math::UnitQuaternion<type>;
using vec_t = fcp::math::ColumnVector<type, 3>;

int main()
{
	namespace fcpm = fcp::math;

	//----------------------------------------------------------------------------------
	// Constructors
	//----------------------------------------------------------------------------------
	
	std::cout << "-- Constructors --\n";

	// Default constructor (no at compile-time)

	// Value initialization
	constexpr static quat_t q_v{};

	// List constructors
	// 1. Proper
	constexpr static quat_t q_l{
		1.f, 2.f, 3.f, 4.f
	};

	// 2.1. Scalar - Vector (temporaries)
	constexpr static quat_t q_svt{
		1.f, vec_t{ 2.f, 3.f, 4.f }
	};

	// 2.2 Scalar - Vector (references)
	constexpr static type temp{ 1.f };
	constexpr static vec_t vec{ 2.f, 3.f, 4.f };
	constexpr static quat_t q_svr{
		temp, vec
	};

	// Broadcasting constructor
	constexpr static quat_t q_brd(3.f);

	static_assert(q_brd[0] == 3.f);

	fcpm::for_constexpr<0, q_brd.size()>(
		[&](auto i){
			static_assert(q_brd[i] == 3.f);
		}
	);

	// Copy constructor
	constexpr static quat_t q_c{ q_l };

	// Const quaternion: not tested, constexpr implies const

	// Test identity quaternion
	constexpr static auto identity{ quat_t::identity() };

	static_assert(q_c == (q_c*identity));

	// Unit quaternion for later
	constexpr static uquat_t uq{ 0.489f, 0.f, 0.f, -0.872f };

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Accessors
	//----------------------------------------------------------------------------------

	std::cout << "-- Accessors --\n";

	// Simple expression template to test accessors on it, too
	constexpr auto q_expr{ q_c + q_v };

	// Test accessor method to get underlying buffer
	constexpr static auto buf{ q_c.data() };

	// Test components' accessors
	static_assert(q_c.scalar() == q_expr.scalar());
	
	fcpm::for_constexpr<0, 2>([&](auto i){
		static_assert(q_c.vector()[i] == q_expr.vector()[i]);
	});

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Arithmetic operators
	//----------------------------------------------------------------------------------

	std::cout << "-- Arithmetic operators --\n";

	constexpr auto q_add{ q_l + q_v };

	static_assert(q_add[0] == 1.);

	quat_t q = q_add;

	constexpr auto op1{ q_add + q_l + q_add };
	constexpr auto op2{ op1 + q_l + q_add };

	static_assert(op2[2] == 15.);

	constexpr auto q_sub{ q_l - q_c };

	static_assert(q_sub[1] == 0.);

	constexpr auto q_brd_mul{ q_l * 3 };

	static_assert(q_brd_mul[0] == 3.);

	constexpr auto q_brd_add{ q_l + 4. };

	static_assert(q_brd_add[0] == 5.);

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Logical operators
	//----------------------------------------------------------------------------------

	std::cout << "-- Logical operators --\n";

	static_assert(q_l == q_c);
	static_assert(!(q_l != q_c));

	std::cout << FCPG_FG_GREEN << "Passed!\n" << FCPG_RESET;

	//----------------------------------------------------------------------------------
	// Quaternion operations
	//----------------------------------------------------------------------------------

	// Conjugation
	constexpr auto q_conj{ q_l.conjugate() };
	constexpr auto uq_conj{ uq.conjugate() };

	fcpm::for_constexpr<0, 2>([&](auto i){
		static_assert(-q_l.vector()[i] == q_conj.vector()[i]);
	});

	// Hamilton product
	constexpr auto q_ham{ q_l * q_l };

	static_assert(q_ham[0] == -28.);
	static_assert(q_ham[1] == 4.);
	static_assert(q_ham[2] == 6.);
	static_assert(q_ham[3] == 8.);

	// Inverse
	constexpr auto q_inv{ q_l.inverse() };
	constexpr auto uq_inv{ uq.inverse() };

	// L2-norm
	constexpr auto q_norm{ fcpm::l2norm(uq) };
	static_assert(fcpm::cmp(q_norm, 1.f));

	// Squared L2-norm
	constexpr auto q_sqnorm{ fcpm::l2norm_sq(uq) };
	static_assert(fcpm::cmp(q_sqnorm, 1.f));

	// Normalization
	static_assert(fcpm::cmp(
			fcpm::l2norm_sq(fcpm::normalize(q_l)), 
			1.f
	));

	// Re-normalize unit quaternion
	fcpm::UnitQuaternion<type> unq{ 
		static_cast<type>(0),
		static_cast<type>(0.3),
		static_cast<type>(0.3),
		static_cast<type>(0.3)
	};
	std::cout << unq << '\n';
	unq.renormalize();
	std::cout << unq << '\n';

	constexpr type pio4{ std::numbers::pi_v<type> / static_cast<type>(4) };
	constexpr static uquat_t uq_pio4{ pio4, pio4, static_cast<type>(0), static_cast<type>(0) };

	// Logarithm
	constexpr auto ln_uqp4{ fcpm::ln(uq_pio4) };
	constexpr static uquat_t ln_expected{ 0.f, pio4, 0.f, 0.f };
	//static_assert(fcpm::cmp(ln_uqp4[1], ln_expected[1]));
	//static_assert(fcpm::ln(uq_pio4) == uquat_t{ 0.f, pio4, 0.f, 0.f });
	
	// Exponential
	
	//constexpr auto uqel{ fcpm::exp(fcpm::ln(uq)) };
	//static_assert(fcpm::cmp(fcpm::l2norm_sq(uqel), fcpm::l2norm_sq(uq)));
	//static_assert(fcpm::exp(fcpm::ln(uq)) == uq);

	// Power
	constexpr auto uqp1o3{ fcpm::pow(uq_pio4, 1.f/3.f) };
	constexpr static uquat_t p_expected{ 0.9659f, 0.2588f, 0.f, 0.f };

	//static_assert(uqp1o3 == p_expected);
	
	// Angular displacement
	static_assert(fcpm::quat_diff(uquat_t::identity(), uq_pio4) == uq_pio4);
}
