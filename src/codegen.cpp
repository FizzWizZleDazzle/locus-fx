//
// Created by artur on 1/8/26.
//

#include "codegen.h"
#include <string>
#include <symengine/real_double.h>
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/constants.h>
#include <symengine/functions.h>
#include <numbers>

namespace locusfx {
    using namespace std;
    using namespace SymEngine;

    namespace {
        // Helper for single-argument functions
        template<typename T>
        string unaryFunc(const RCP<const Basic> &expr, const string &funcName) {
            return funcName + "(" + toWGSL(rcp_static_cast<const T>(expr)->get_arg()) + ")";
        }

        // Helper for reciprocal functions: 1/f(x)
        template<typename T>
        string reciprocalFunc(const RCP<const Basic> &expr, const string &funcName) {
            return "(1.0 / " + funcName + "(" + toWGSL(rcp_static_cast<const T>(expr)->get_arg()) + "))";
        }

        // Helper for inverse reciprocal functions: f(1/x)
        template<typename T>
        string inverseReciprocalFunc(const RCP<const Basic> &expr, const string &funcName) {
            return funcName + "(1.0 / " + toWGSL(rcp_static_cast<const T>(expr)->get_arg()) + ")";
        }
    }

    string toWGSL(const RCP<const Basic> &expr) {
        //Recursively traverse the expression tree and convert to WGSL syntax

        // Checks if the node is an integer
        if (is_a<Integer>(*expr)) {
            auto num = rcp_static_cast<const Integer>(expr)->as_int();
            return to_string(num) + ".0";
        }
        if (is_a<RealDouble>(*expr)) {
            ostringstream oss;
            oss << rcp_static_cast<const RealDouble>(expr)->i;
            return oss.str();
        }
        if (is_a<Rational>(*expr)) {
            auto rat = rcp_static_cast<const Rational>(expr);
            auto num = rat->get_num()->as_int();
            auto den = rat->get_den()->as_int();
            return "(" + to_string(num) + ".0 / " + to_string(den) + ".0 )";
        }
        if (is_a<Symbol>(*expr)) {
            return rcp_static_cast<const Symbol>(expr)->get_name();
        }
        if (is_a<Constant>(*expr)) {
            auto name = rcp_static_cast<const Constant>(expr)->get_name();
            if (name == "pi") return to_string(numbers::pi);
            if (name == "E") return to_string(numbers::e);
            throw runtime_error("Unknown constant: " + name);
        }

        // Addition: a + b + c + ...
        if (is_a<Add>(*expr)) {
            auto add = rcp_static_cast<const Add>(expr);
            string result = "(";
            bool first = true;

            // Handle the coefficient (constant term)
            if (!is_a<Integer>(*add->get_coef()) ||
                rcp_static_cast<const Integer>(add->get_coef())->as_int() != 0) {
                result += toWGSL(add->get_coef());
                first = false;
            }

            // Handle each term in the sum
            for (const auto &term : add->get_dict()) {
                if (!first) result += " + ";
                // term is pair<RCP<Basic>, RCP<Number>> where value is coefficient
                if (is_a<Integer>(*term.second) &&
                    rcp_static_cast<const Integer>(term.second)->as_int() == 1) {
                    result += toWGSL(term.first);
                } else if (is_a<Integer>(*term.second) &&
                           rcp_static_cast<const Integer>(term.second)->as_int() == -1) {
                    result += "(-" + toWGSL(term.first) + ")";
                } else {
                    result += "(" + toWGSL(term.second) + " * " + toWGSL(term.first) + ")";
                }
                first = false;
            }
            result += ")";
            return result;
        }

        // Multiplication: a * b * c * ...
        if (is_a<Mul>(*expr)) {
            auto mul = rcp_static_cast<const Mul>(expr);
            string result = "(";
            bool first = true;

            // Handle the coefficient
            if (!is_a<Integer>(*mul->get_coef()) ||
                rcp_static_cast<const Integer>(mul->get_coef())->as_int() != 1) {
                result += toWGSL(mul->get_coef());
                first = false;
            }

            // Handle each factor: dict contains base -> exponent pairs
            for (const auto &factor : mul->get_dict()) {
                if (!first) result += " * ";
                // factor.first is base, factor.second is exponent
                if (is_a<Integer>(*factor.second) &&
                    rcp_static_cast<const Integer>(factor.second)->as_int() == 1) {
                    result += toWGSL(factor.first);
                } else {
                    result += "pow(" + toWGSL(factor.first) + ", " + toWGSL(factor.second) + ")";
                }
                first = false;
            }
            result += ")";
            return result;
        }

        // Power: base^exponent
        if (is_a<Pow>(*expr)) {
            auto pw = rcp_static_cast<const Pow>(expr);
            auto base = pw->get_base();
            auto exp = pw->get_exp();

            // Special case: sqrt when exponent is 1/2
            if (is_a<Rational>(*exp)) {
                auto rat = rcp_static_cast<const Rational>(exp);
                if (rat->get_num()->as_int() == 1 && rat->get_den()->as_int() == 2) {
                    return "sqrt(" + toWGSL(base) + ")";
                }
            }

            return "pow(" + toWGSL(base) + ", " + toWGSL(exp) + ")";
        }

        // Trigonometric functions
        if (is_a<Sin>(*expr)) return unaryFunc<Sin>(expr, "sin");
        if (is_a<Cos>(*expr)) return unaryFunc<Cos>(expr, "cos");
        if (is_a<Tan>(*expr)) return unaryFunc<Tan>(expr, "tan");
        if (is_a<Csc>(*expr)) return reciprocalFunc<Csc>(expr, "sin");
        if (is_a<Sec>(*expr)) return reciprocalFunc<Sec>(expr, "cos");
        if (is_a<Cot>(*expr)) return reciprocalFunc<Cot>(expr, "tan");

        // Inverse trigonometric functions
        if (is_a<ASin>(*expr)) return unaryFunc<ASin>(expr, "asin");
        if (is_a<ACos>(*expr)) return unaryFunc<ACos>(expr, "acos");
        if (is_a<ATan>(*expr)) return unaryFunc<ATan>(expr, "atan");
        if (is_a<ACsc>(*expr)) return inverseReciprocalFunc<ACsc>(expr, "asin");
        if (is_a<ASec>(*expr)) return inverseReciprocalFunc<ASec>(expr, "acos");
        if (is_a<ACot>(*expr)) return inverseReciprocalFunc<ACot>(expr, "atan");

        // Hyperbolic functions
        if (is_a<Sinh>(*expr)) return unaryFunc<Sinh>(expr, "sinh");
        if (is_a<Cosh>(*expr)) return unaryFunc<Cosh>(expr, "cosh");
        if (is_a<Tanh>(*expr)) return unaryFunc<Tanh>(expr, "tanh");
        if (is_a<Csch>(*expr)) return reciprocalFunc<Csch>(expr, "sinh");
        if (is_a<Sech>(*expr)) return reciprocalFunc<Sech>(expr, "cosh");
        if (is_a<Coth>(*expr)) return reciprocalFunc<Coth>(expr, "tanh");

        // Inverse hyperbolic functions
        if (is_a<ASinh>(*expr)) return unaryFunc<ASinh>(expr, "asinh");
        if (is_a<ACosh>(*expr)) return unaryFunc<ACosh>(expr, "acosh");
        if (is_a<ATanh>(*expr)) return unaryFunc<ATanh>(expr, "atanh");
        if (is_a<ACsch>(*expr)) return inverseReciprocalFunc<ACsch>(expr, "asinh");
        if (is_a<ASech>(*expr)) return inverseReciprocalFunc<ASech>(expr, "acosh");
        if (is_a<ACoth>(*expr)) return inverseReciprocalFunc<ACoth>(expr, "atanh");

        // Logarithm and exponential
        if (is_a<Log>(*expr)) return unaryFunc<Log>(expr, "log");

        // Other common functions
        if (is_a<Abs>(*expr)) return unaryFunc<Abs>(expr, "abs");
        if (is_a<Sign>(*expr)) return unaryFunc<Sign>(expr, "sign");
        if (is_a<Floor>(*expr)) return unaryFunc<Floor>(expr, "floor");
        if (is_a<Ceiling>(*expr)) return unaryFunc<Ceiling>(expr, "ceil");

        // Unsupported expression type
        throw runtime_error("Unsupported expression type: " + expr->__str__());
    }
}