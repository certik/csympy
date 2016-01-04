#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/polynomial.h>
#include <symengine/complex_double.h>
#include <symengine/complex_mpc.h>

namespace SymEngine {

extern RCP<const Basic> i2;

class DiffImplementation {
public:
    /*
    static RCP<const Basic> diff(const Basic &self,
            const RCP<const Symbol> &x) {
        return Derivative::create(self.rcp_from_this(), {x});
    }
    */

    static RCP<const Basic> diff(const Number &self,
            const RCP<const Symbol> &x) {
        return zero;
    }

    static RCP<const Basic> diff(const Constant &self,
            const RCP<const Symbol> &x) {
        return zero;
    }

    static RCP<const Basic> diff(const Symbol &self,
            const RCP<const Symbol> &x) {
        if (x->get_name() == self.get_name())
            return one;
        else
            return zero;
    }

    static RCP<const Basic> diff(const Log &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Abs &self,
            const RCP<const Symbol> &x) {
        if (eq(*self.get_arg()->diff(x), *zero))
            return zero;
        else
            return Derivative::create(self.rcp_from_this(), {x});
    }

#define DIFF0(CLASS) \
static RCP<const Basic> diff(const CLASS &self, \
        const RCP<const Symbol> &x) { \
    return zero; \
}

    DIFF0(UnivariatePolynomial)
    DIFF0(UnivariateSeries)
    DIFF0(Sinh)
    DIFF0(Cosh)
    DIFF0(Tanh)
    DIFF0(Coth)
    DIFF0(ASinh)
    DIFF0(ACosh)
    DIFF0(ATanh)
    DIFF0(ACoth)
    DIFF0(ASech)
    DIFF0(LambertW)
    DIFF0(Zeta)
    DIFF0(KroneckerDelta)
    DIFF0(Dirichlet_eta)
    DIFF0(FunctionWrapper)
    DIFF0(UpperGamma)
    DIFF0(LowerGamma)
    DIFF0(Gamma)
    DIFF0(LeviCivita)
    DIFF0(Derivative)
    DIFF0(FunctionSymbol)
    DIFF0(Subs)

    static RCP<const Basic> diff(const Add &self,
            const RCP<const Symbol> &x) {
        SymEngine::umap_basic_num d;
        RCP<const Number> coef=zero, coef2;
        RCP<const Basic> t;
        for (auto &p: self.dict_) {
            RCP<const Basic> term = p.first->diff(x);
            if (is_a<Integer>(*term) && rcp_static_cast<const Integer>(term)->is_zero()) {
                continue;
            } else if (is_a_Number(*term)) {
                iaddnum(outArg(coef),
                        mulnum(p.second, rcp_static_cast<const Number>(term)));
            } else if (is_a<Add>(*term)) {
                for (auto &q: (rcp_static_cast<const Add>(term))->dict_)
                    Add::dict_add_term(d, mulnum(q.second, p.second), q.first);
                iaddnum(outArg(coef), mulnum(p.second, rcp_static_cast<const Add>(term)->coef_));
            } else {
                Add::as_coef_term(mul(p.second, term), outArg(coef2), outArg(t));
                Add::dict_add_term(d, coef2, t);
            }
        }
        return Add::from_dict(coef, std::move(d));
    }

    static RCP<const Basic> diff(const Mul &self,
            const RCP<const Symbol> &x) {
        RCP<const Number> overall_coef = zero;
        umap_basic_num add_dict;
        for (auto &p: self.dict_) {
            RCP<const Number> coef = self.coef_;
            RCP<const Basic> factor = pow(p.first, p.second)->diff(x);
            if (is_a<Integer>(*factor) &&
                    rcp_static_cast<const Integer>(factor)->is_zero()) continue;
            map_basic_basic d = self.dict_;
            d.erase(p.first);
            if (is_a_Number(*factor)) {
                imulnum(outArg(coef), rcp_static_cast<const Number>(factor));
            } else if (is_a<Mul>(*factor)) {
                RCP<const Mul> tmp = rcp_static_cast<const Mul>(factor);
                imulnum(outArg(coef), tmp->coef_);
                for (auto &q: tmp->dict_) {
                    Mul::dict_add_term_new(outArg(coef), d, q.second, q.first);
                }
            } else {
                RCP<const Basic> exp, t;
                Mul::as_base_exp(factor, outArg(exp), outArg(t));
                Mul::dict_add_term_new(outArg(coef), d, exp, t);
            }
            if (d.size() == 0) {
                iaddnum(outArg(overall_coef), coef);
            } else {
                RCP<const Basic> mul = Mul::from_dict(one, std::move(d));
                Add::dict_add_term(add_dict, coef, mul);
            }
        }
        return Add::from_dict(overall_coef, std::move(add_dict));
    }

    static RCP<const Basic> diff(const Pow &self,
            const RCP<const Symbol> &x) {
        if (is_a_Number(*(self.get_exp())))
            return mul(mul(self.get_exp(), pow(self.get_base(), sub(self.get_exp(), one))), self.get_base()->diff(x));
        else
            return mul(pow(self.get_base(), self.get_exp()), mul(self.get_exp(), log(self.get_base()))->diff(x));
    }

    static RCP<const Basic> diff(const Sin &self,
            const RCP<const Symbol> &x) {
        return mul(cos(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cos &self,
            const RCP<const Symbol> &x) {
        return mul(mul(minus_one, sin(self.get_arg())),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Tan &self,
            const RCP<const Symbol> &x) {
        RCP<const Integer> two = integer(2);
        return mul(add(one, pow(tan(self.get_arg()), two)),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cot &self,
            const RCP<const Symbol> &x) {
        RCP<const Integer> two = integer(2);
        return mul(mul(add(one, pow(cot(self.get_arg()), two)), minus_one),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Csc &self,
            const RCP<const Symbol> &x) {
        return mul(mul(mul(cot(self.get_arg()), csc(self.get_arg())),
                    minus_one), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Sec &self,
            const RCP<const Symbol> &x) {
        return mul(mul(tan(self.get_arg()), sec(self.get_arg())),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASin &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, sqrt(sub(one, pow(self.get_arg(), i2)))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACos &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, sqrt(sub(one, pow(self.get_arg(), i2)))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ASec &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, mul(pow(self.get_arg(), i2),
                sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACsc &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, mul(pow(self.get_arg(), i2),
                sqrt(sub(one, div(one, pow(self.get_arg(), i2)))))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATan &self,
            const RCP<const Symbol> &x) {
        return mul(div(one, add(one, pow(self.get_arg(), i2))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ACot &self,
            const RCP<const Symbol> &x) {
        return mul(div(minus_one, add(one, pow(self.get_arg(), i2))),
                self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const ATan2 &self,
            const RCP<const Symbol> &x) {
        return mul(div(pow(self.get_den(), i2), add(pow(self.get_den(), i2),
            pow(self.get_num(), i2))), div(self.get_num(),
            self.get_den())->diff(x));
    }

    // ...and so on for all the other functions that want to return a specific
    // derivative, instead of the general diff(const Basic &, ...) one.
};

#define DIFF2(CLASS) \
RCP<const Basic> CLASS::diff2(const RCP<const Symbol> &x) const { \
    return DiffImplementation::diff(*this, x); \
};


#define SYMENGINE_ENUM(TypeID, Class) DIFF2(Class)
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM


#define IMPLEMENT_DIFF(CLASS) \
RCP<const Basic> CLASS::diff(const RCP<const Symbol> &x) const \
{ \
    return Derivative::create(rcp_from_this(), {x}); \
}

#define IMPLEMENT_DIFF0(CLASS) \
RCP<const Basic> CLASS::diff(const RCP<const Symbol> &x) const \
{ \
    return zero; \
}

IMPLEMENT_DIFF(FunctionWrapper)
IMPLEMENT_DIFF(UpperGamma)
IMPLEMENT_DIFF(LowerGamma)
IMPLEMENT_DIFF(Gamma)
IMPLEMENT_DIFF(LeviCivita)
IMPLEMENT_DIFF(KroneckerDelta)
IMPLEMENT_DIFF(Dirichlet_eta)
IMPLEMENT_DIFF(UnivariateSeries)

IMPLEMENT_DIFF0(RealDouble)
IMPLEMENT_DIFF0(ComplexDouble)
IMPLEMENT_DIFF0(Complex)
IMPLEMENT_DIFF0(Rational)
IMPLEMENT_DIFF0(Integer)
IMPLEMENT_DIFF0(NumberWrapper)


} // SymEngine