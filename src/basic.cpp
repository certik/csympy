#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "constants.h"
#include "functions.h"

namespace CSymPy {

int Basic::__cmp__(const Basic &o) const
{
    auto a = this->get_type_code();
    auto b = o.get_type_code();
    if (a == b) {
        return this->compare(o);
    } else {
        // We return the order given by the numerical value of the TypeID enum type.
        // The types don't need to be ordered in any given way, they just need to be ordered.
        return a < b ? -1 : 1;
    }
}

std::string Basic::__str__() const
{
    std::ostringstream s;
    s << "<" << typeName<Basic>(*this)
        << " instance at " << (const void*)this << ">";
    return s.str();
}

RCP<const Basic> expand(const RCP<const Basic> &self)
{
    if (is_a<Symbol>(*self)) return self;
    if (is_a_Number(*self)) return self;
    if (is_a<Add>(*self)) return add_expand(rcp_static_cast<const Add>(self));
    if (is_a<Mul>(*self)) return mul_expand(rcp_static_cast<const Mul>(self));
    if (is_a<Pow>(*self)) return pow_expand(rcp_static_cast<const Pow>(self));
    return self;
}

RCP<const Basic> Basic::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Basic> self = rcp_const_cast<Basic>(rcp(this));
    auto it = subs_dict.find(self);
    if (it == subs_dict.end())
        return self;
    else
        return it->second;
}

RCP<const Basic> Basic::diff(const RCP<const Symbol> &x) const
{
    return rcp(new Derivative(rcp(this), {x}));
}

struct Object;
struct ObjectKeyLess;

typedef std::map<Object, Object, ObjectKeyLess> map_object_object;

struct AddS {
    RCP<const Number> coef_;
    umap_basic_num dict_;
    std::vector<Object> v; // This works
};
struct MulS {
    RCP<const Number> coef_;
    map_basic_basic dict_;
    // map_object_object dict2_; // error: invalid use of incomplete type ‘struct CSymPy::ObjectKeyLess’
};
struct PowS {
    RCP<const Basic> base_, exp_;
};
struct IntegerS {
    mpz_class i;
};
struct RationalS {
    mpq_class i;
};
struct ComplexS {
    mpq_class real_;
    mpq_class imaginary_;
};
struct SymbolS {
    std::string name_;
};


struct Object {
    Object() {}

    Object(Object const&) = delete;
    Object& operator=(Object const&) = delete;

    Object(Object&&) = delete;
    Object& operator=(Object&&) = delete;

    ~Object() {}

    TypeID type_id;
    union {
        AddS a;
        MulS m;
        PowS p;
        IntegerS i;
        RationalS r;
        ComplexS c;
        SymbolS s;
        RCP<const Basic> b;
    };
};

//! Our less operator `(<)`:
struct ObjectKeyLess {
    //! true if `x < y`, false otherwise
    bool operator() (const Object &x, const Object &y) const {
        /*
        std::size_t xh=x->hash(), yh=y->hash();
        if (xh != yh) return xh < yh;
        if (x->__eq__(*y)) return false;
        return x->__cmp__(*y) == -1;
        */
        return x.type_id < y.type_id;
    }
};

void test1()
{
    Object o;
    o.type_id = SYMBOL;
    new (&o.s) SymbolS();
    o.s.name_ = "test";
    std::cout << o.s.name_ << std::endl;
    o.s.~SymbolS();
}

} // CSymPy

