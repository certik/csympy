#include <chrono>
#include <memory>

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

//#define UNIQUE_PTR_DEBUG

template<class T>
class UniquePtr {
public:
    inline explicit UniquePtr( T *ptr ) : ptr_(ptr) {
        CSYMPY_ASSERT(ptr != nullptr)
    }
#ifndef UNIQUE_PTR_DEBUG
    ~UniquePtr() { delete ptr_; }
#endif
    // Copy constructor and assignment are disabled
    inline UniquePtr(const UniquePtr<T>& ptr) = delete;
    UniquePtr<T>& operator=(const UniquePtr<T>& ptr) = delete;
    // Move constructor and assignment
    inline UniquePtr(UniquePtr&&) = default;
    UniquePtr<T>& operator=(UniquePtr&&) = default;
    inline T* operator->() const { return ptr_; }
    inline T& operator*() const { return *ptr_; }
    inline const Ptr<T> ptr() const {
#ifdef UNIQUE_PTR_DEBUG
        return ptr_.ptr();
#else
        return Ptr<T>(ptr_);
#endif
    }
private:
#ifdef UNIQUE_PTR_DEBUG
    RCP<T> ptr_;
#else
    T *ptr_;
#endif
};

template<class T>
inline UniquePtr<T> uniqueptr(T* p)
{
    return UniquePtr<T>(p);
}

//! Our less operator `(<)`:
struct UniquePtrBasicKeyLess {
    //! true if `x < y`, false otherwise
    bool operator() (const std::unique_ptr<const Basic> &x, const std::unique_ptr<const Basic> &y) const {
        std::size_t xh=x->hash(), yh=y->hash();
        if (xh != yh) return xh < yh;
        if (x->__eq__(*y)) return false;
        return x->__cmp__(*y) == -1;
    }
};

typedef std::map<std::unique_ptr<const Basic>, std::unique_ptr<const Basic>, UniquePtrBasicKeyLess> map_basic_basic_unique_ptr;


struct Object {
    TypeID type_id;
    // Put the largest object here. The assert statements below check this at
    // compile time, if they fail, then 'largest_object' was not the largest
    // and one has to fix this:
    //using largest_object = Add;
    using largest_object = Integer;
    alignas(Basic) char data[sizeof(largest_object)];
};
#define check_size_alignment(TYPE) \
    static_assert(sizeof(TYPE) <= sizeof(Object::data), "Size of 'Object' is not correct"); \
    static_assert(std::alignment_of<TYPE>::value == std::alignment_of<Object>::value, "Alignment of 'Object' is not correct");

check_size_alignment(Basic)
//check_size_alignment(Add)
//check_size_alignment(Mul)
check_size_alignment(Pow)
check_size_alignment(Integer)
//check_size_alignment(Rational)
//check_size_alignment(Complex)
check_size_alignment(Symbol)
check_size_alignment(RCP<const Basic>)

//! Our less operator `(<)`:
struct ObjectKeyLess {
    //! true if `x < y`, false otherwise
    bool operator() (const Object &x_, const Object &y_) const {
        const Basic *x = reinterpret_cast<const Basic *>(x_.data);
        const Basic *y = reinterpret_cast<const Basic *>(y_.data);
        std::size_t xh=x->hash(), yh=y->hash();
        if (xh != yh) return xh < yh;
        if (x->__eq__(*y)) return false;
        return x->__cmp__(*y) == -1;
    }
};
typedef std::map<Object, Object, ObjectKeyLess> map_object_object;

struct Object2;
struct ObjectKeyLess2;

typedef std::map<Object2, Object2, ObjectKeyLess2> map_object_object2;

struct AddS {
    RCP<const Number> coef_;
    umap_basic_num dict_;
    std::vector<Object2> v; // This works
};
struct MulS {
    RCP<const Number> coef_;
    map_basic_basic dict_;
    // map_object_object2 dict2_; // error: invalid use of incomplete type ‘struct CSymPy::ObjectKeyLess2’
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


struct Object2 {
    Object2() {}

    Object2(Object2 const&) = delete;
    Object2& operator=(Object2 const&) = delete;

    Object2(Object2&&) = delete;
    Object2& operator=(Object2&&) = delete;

    ~Object2() {}

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
struct ObjectKeyLess2 {
    //! true if `x < y`, false otherwise
    bool operator() (const Object2 &x, const Object2 &y) const {
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
    Object2 o;
    o.type_id = SYMBOL;
    new (&o.s) SymbolS();
    o.s.name_ = "test";
    std::cout << o.s.name_ << std::endl;
    o.s.~SymbolS();


    std::cout << sizeof(Object::type_id) << std::endl;;

    //int max_n = 10000000;
    int max_n = 10;

    map_basic_basic d;
    std::cout << "start RCP" << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i=0; i < max_n; i++) {
        insert(d, rcp(new Integer(i)), rcp(new Integer(i+1)));
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "stop RCP" << std::endl;
//    std::cout << d << std::endl;
    std::cout << "Time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;

    map_object_object d2;
    std::cout << "start value" << std::endl;
    t1 = std::chrono::high_resolution_clock::now();
    for (int i=0; i < max_n; i++) {
        Object i1, i2;
        i1.type_id = INTEGER;
        i2.type_id = INTEGER;
        new (i1.data) Integer(i);
        new (i2.data) Integer(i+1);
        insert(d2, i1, i2);
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "stop value" << std::endl;
    for (auto &p: d2) {
        const Integer *i = reinterpret_cast<const Integer *>(p.first.data);
        const Integer *j = reinterpret_cast<const Integer *>(p.second.data);
        std::cout << *i << ":" << *j << std::endl;
    }
    std::cout << "Time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;

    map_basic_basic_unique_ptr d3;
    std::cout << "start value" << std::endl;
    t1 = std::chrono::high_resolution_clock::now();
    for (int i=0; i < max_n; i++) {
        std::unique_ptr<const Basic> i1(new Integer(i));
        std::unique_ptr<const Basic> i2(new Integer(i+1));
        d3.insert(std::pair<std::unique_ptr<const Basic>, std::unique_ptr<const Basic>>(std::move(i1), std::move(i2)));
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "stop value" << std::endl;
    for (auto &p: d3) {
        std::cout << *(p.first) << ":" << *(p.second) << std::endl;
    }
    std::cout << "Time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << std::endl;
}

} // CSymPy

