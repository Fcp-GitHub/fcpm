#include "math_types/lazy/expression_templates.hpp"

// Taken from: https://www.reddit.com/r/cpp/comments/ijtokq/an_alternative_to_stdtype_info_and_typeid_with/

#include <iostream>
#include <memory>
#include <cxxabi.h>
template<typename T>
std::string type_name()
{
    int status = 0;

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status),
        std::free
    };

    if (status != 0) throw status; // stub

    return res.get();
}

int main()
{
	using namespace fcp::math::lazy;
	using Double = Scalar<double>;

	constexpr Double g{ 9.81 };
	constexpr Double x{ 4 }, vx{ 2 }, t{ 3 };

	// Expression: x + v*t - 0.5*g*t*t
	constexpr auto v_t{ MulExpr<Double, Double, double>(vx, t) };
	constexpr auto x_vt{ SumExpr<Double, decltype(v_t), double>(x, v_t) };
	constexpr auto t_sq{ MulExpr<Double, Double, double>(t, t) };
	constexpr auto g_tt{ MulExpr<Double, decltype(t_sq), double>(g, t_sq) };
	constexpr auto h_gtt{ MulExpr<Double, decltype(g_tt), double>(0.5, g_tt) };
	constexpr auto a{ SubExpr<decltype(x_vt), decltype(h_gtt), double>(x_vt, h_gtt) };

	// Assertion fails with:
	//constexpr auto h_g{ MulExpr<Double, Double, double>(0.5, g) };
	//constexpr auto hg_t{ MulExpr<decltype(h_g), Double, double>(h_g, t) };
	//constexpr auto hgt_t{ MulExpr<decltype(hg_t), Double, double>(hg_t, t) };
	//constexpr auto a{ SubExpr<decltype(x_vt), decltype(hgt_t), double>(x_vt, hgt_t) };

	std::cout << "type of a: " << type_name<decltype(a)>() << '\n';

	static_assert(a.evaluate(0) == -34.145);

	// Same expression, but summing (x+v*t) and (-0.5*g*t*t)
	constexpr auto n_hgtt{ NegExpr<decltype(h_gtt), double>(h_gtt) };
	constexpr auto neg_a{ SumExpr<decltype(x_vt), decltype(n_hgtt), double>(x_vt, n_hgtt) };

	static_assert(neg_a.evaluate(0) == -34.145);

	// Expression: (+x) - (+x)
	constexpr auto px{ IdeExpr<Double, double>(x) };
	constexpr auto px_px{ SubExpr<decltype(px), decltype(px), double>(px, px) };

	static_assert(px_px.evaluate(0) == 0);

	// Expression (x - x) == 0
	constexpr auto zero{ SubExpr<Double, Double, double>(x, x) };
	constexpr auto eq_z{ EqToExpr<decltype(zero), Double, double>(zero, 0.) };

	static_assert(eq_z.evaluate(0));

	// Same expression, but with negation
	constexpr auto neq_z{ NotEqToExpr<decltype(zero), Double, double>(zero, 0) };

	static_assert(!neq_z.evaluate(0));

	// Expression: (x + 1) >= x
	constexpr auto x_po{ SumExpr<Double, Double, double>(x, 1) };
	constexpr auto get_x{ GreaterEqExpr<decltype(x_po), Double, double>(x_po, x) };

	static_assert(get_x.evaluate(0));

	// Expression: (x + 1) > x
	constexpr auto gt_x{ GreaterExpr<decltype(x_po), Double, double>(x_po, x) };

	static_assert(gt_x.evaluate(0));

	// Expression: (x - 1) <= x
	constexpr auto x_lo{ SubExpr<Double, Double, double>(x, 1) };
	constexpr auto let_x{ LessEqExpr<decltype(x_lo), Double, double>(x_lo, x) };

	static_assert(let_x.evaluate(0));

	// Expression: (x - 1) < x
	constexpr auto lt_x{ LessExpr<decltype(x_lo), Double, double>(x_lo, x) };

	static_assert(lt_x.evaluate(0));
}
