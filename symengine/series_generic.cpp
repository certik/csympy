#include <algorithm>
#include <exception>
#include <iterator>
#include <symengine/series_generic.h>
#include <symengine/series_visitor.h>

using SymEngine::RCP;
using SymEngine::make_rcp;

namespace SymEngine
{

RCP<const UnivariateSeries> UnivariateSeries::series(const RCP<const Basic> &t,
                                                     const std::string &x,
                                                     unsigned int prec)
{
    UExprODict p({{1, Expression(1)}});
    SeriesVisitor<UExprODict, Expression, UnivariateSeries> visitor(
        std::move(p), x, prec);
    return visitor.series(t);
}

std::size_t UnivariateSeries::__hash__() const
{
    std::size_t seed = UEXPRPOLYO;
    for (const auto &it : p_.dict_) {
        std::size_t temp = UEXPRPOLYO;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<Basic>(temp, *(it.second.get_basic()));
        seed += temp;
    }
    return seed + std::size_t(get_degree() * 84728863L);
}

int UnivariateSeries::compare(const Basic &other) const
{
    SYMENGINE_ASSERT(is_a<UnivariateSeries>(other))
    const UnivariateSeries &o_ = static_cast<const UnivariateSeries &>(other);
    return p_.compare(o_.get_poly());
}

RCP<const Basic> UnivariateSeries::as_basic() const
{
    return p_.get_basic(var_);
}

umap_int_basic UnivariateSeries::as_dict() const
{
    umap_int_basic map;
    for (const auto &it : p_.get_dict())
        if (it.second != 0)
            map[it.first] = it.second.get_basic();
    return map;
}

RCP<const Basic> UnivariateSeries::get_coeff(int deg) const
{
    if (p_.get_dict().count(deg) == 0)
        return zero;
    else
        return p_.get_dict().at(deg).get_basic();
}

UExprODict UnivariateSeries::var(const std::string &s)
{
    return UExprODict({{1, Expression(1)}});
}

Expression UnivariateSeries::convert(const Basic &x)
{
    return Expression(x.rcp_from_this());
}

int UnivariateSeries::ldegree(const UExprODict &s)
{
    return s.get_dict().begin()->first;
}

UExprODict UnivariateSeries::mul(const UExprODict &a, const UExprODict &b,
                                 unsigned prec)
{
    map_int_Expr p;
    for (auto &it1 : a.get_dict()) {
        for (auto &it2 : b.get_dict()) {
            int exp = it1.first + it2.first;
            if (exp < (int)prec) {
                p[exp] += it1.second * it2.second;
            } else {
                break;
            }
        }
    }
    return UExprODict(p);
}

UExprODict UnivariateSeries::pow(const UExprODict &base, int exp, unsigned prec)
{
    if (exp < 0) {
        SYMENGINE_ASSERT(base.size() == 1)
        map_int_Expr dict;
        dict[-(base.get_dict().begin()->first)]
            = 1 / base.get_dict().begin()->second;
        return pow(UExprODict(dict), -exp, prec);
    }
    if (exp == 0) {
        if (base == 0 or base.get_dict().size() == 0) {
            throw std::runtime_error("Error: 0**0 is undefined.");
        } else {
            return UExprODict(1);
        }
    }

    UExprODict x(base);
    UExprODict y(1);
    while (exp > 1) {
        if (exp % 2 == 0) {
            x = mul(x, x, prec);
            exp /= 2;
        } else {
            y = mul(x, y, prec);
            x = mul(x, x, prec);
            exp = (exp - 1) / 2;
        }
    }
    return mul(x, y, prec);
}

Expression UnivariateSeries::find_cf(const UExprODict &s, const UExprODict &var,
                                     int deg)
{
    if (s.get_dict().count(deg) == 0)
        return Expression(0);
    else
        return (s.get_dict()).at(deg);
}

Expression UnivariateSeries::root(Expression &c, unsigned n)
{
    return pow_ex(c, 1 / Expression(n));
}

UExprODict UnivariateSeries::diff(const UExprODict &s, const UExprODict &var)
{
    if (var.get_dict().size() == 1 and var.get_dict().at(1) == Expression(1)) {
        map_int_Expr d;
        for (const auto &p : s.get_dict()) {
            if (p.first != 0)
                d[p.first - 1] = p.second * p.first;
        }
        return UExprODict(d);
    } else {
        return UExprODict({{0, Expression(0)}});
    }
}

UExprODict UnivariateSeries::integrate(const UExprODict &s,
                                       const UExprODict &var)
{
    map_int_Expr dict;
    for (auto &it : s.get_dict()) {
        if (it.first != -1) {
            dict.insert(std::pair<int, Expression>(it.first + 1,
                                                   it.second / (it.first + 1)));
        } else {
            throw std::runtime_error("Not Implemented");
        }
    }

    return UExprODict(dict);
}

UExprODict UnivariateSeries::subs(const UExprODict &s, const UExprODict &var,
                                  const UExprODict &r, unsigned prec)
{
    UExprODict result({{1, Expression(1)}});

    for (auto &i : s.get_dict())
        result += i.second * pow(r, i.first, prec);

    return result;
}

Expression UnivariateSeries::sin(const Expression &c)
{
    return SymEngine::sin(c.get_basic());
}

Expression UnivariateSeries::cos(const Expression &c)
{
    return SymEngine::cos(c.get_basic());
}

Expression UnivariateSeries::tan(const Expression &c)
{
    return SymEngine::tan(c.get_basic());
}

Expression UnivariateSeries::asin(const Expression &c)
{
    return SymEngine::asin(c.get_basic());
}

Expression UnivariateSeries::acos(const Expression &c)
{
    return SymEngine::acos(c.get_basic());
}

Expression UnivariateSeries::atan(const Expression &c)
{
    return SymEngine::atan(c.get_basic());
}

Expression UnivariateSeries::sinh(const Expression &c)
{
    return SymEngine::sinh(c.get_basic());
}

Expression UnivariateSeries::cosh(const Expression &c)
{
    return SymEngine::cosh(c.get_basic());
}

Expression UnivariateSeries::tanh(const Expression &c)
{
    return SymEngine::tanh(c.get_basic());
}

Expression UnivariateSeries::asinh(const Expression &c)
{
    return SymEngine::asinh(c.get_basic());
}

Expression UnivariateSeries::atanh(const Expression &c)
{
    return SymEngine::atanh(c.get_basic());
}

Expression UnivariateSeries::exp(const Expression &c)
{
    return SymEngine::exp(c.get_basic());
}

Expression UnivariateSeries::log(const Expression &c)
{
    return SymEngine::log(c.get_basic());
}

} // SymEngine
