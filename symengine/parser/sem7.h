#ifndef SYMENGINE_PARSER_SEM7_H
#define SYMENGINE_PARSER_SEM7_H

#include <variant>

/*
   Computer 1: 12ms 139ms          127ms (linear)  195 (default new)
   Computer 2: --   78ms   92ms
*/

/*
   gcc 7.4.0
    construction: 103ms
    total: 127ms => count: 24ms
   gcc 9.1.0
    construction: 117ms
    total: 132ms => count: 15ms
    C++2a
    construction: 117ms
    total: 129ms => count: 12ms
    */

#include "alloc.h"
#include "symengine/symengine_casts.h"

namespace SymEngine {

extern Allocator al;

}

using SymEngine::al;

template<typename... LAMBDAS> struct visitors : LAMBDAS... {
  using LAMBDAS::operator()...;
};
template<typename... LAMBDAS> visitors(LAMBDAS... x)->visitors<LAMBDAS...>;


enum BinOpType
{
    Add, Sub, Mul, Div
};

struct Base;

struct BinOp {
    const BinOpType type;
    const Base *left, *right;
    BinOp(BinOpType type, const Base *x, const Base *y)
        : type{type}, left{x}, right{y} {
    }
};

struct Pow {
    const Base *base, *exp;
    Pow(const Base *x, const Base *y)
        : base{x}, exp{y} {
    }
};

struct Symbol {
    const char *name;
    Symbol(const std::string s) : name{s.c_str()} { }
};

struct Integer {
    const char *i;
    Integer(const std::string s) : i{s.c_str()} { }
};

struct Base {
    std::variant<BinOp, Pow, Symbol, Integer> u;
    template<typename A> Base(A &&x) : u{std::move(x)}  {}
};



static int count3(const Base &b) {
    return std::visit(
        visitors{
            [](const Symbol &x) { return 1; },
            [](const BinOp &x) {
                int c = 0;
                c += count3(*x.left);
                c += count3(*x.right);
                return c; },
            [](const Pow &x) {
                int c = 0;
                c += count3(*x.base);
                c += count3(*x.exp);
                return c; },
            [](const auto &x) { return 0; },
        },
        b.u);
}

static int count2(const Base &b) {
    return std::visit([](auto&& x) {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, Symbol>) {
                return 1;
            } else if constexpr (std::is_same_v<T, BinOp>) {
                int c = 0;
                c += count2(*x.left);
                c += count2(*x.right);
                return c;
            } else if constexpr (std::is_same_v<T, Pow>) {
                int c = 0;
                c += count2(*x.base);
                c += count2(*x.exp);
                return c;
            } else {
                return 0;
            }
        }, b.u);
}




template <class Derived>
class BaseWalkVisitor
{
public:
    void visit(const BinOp &x) {
        apply(*x.left);
        apply(*x.right);
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    void visit(const Pow &x) {
        apply(*x.base);
        apply(*x.exp);
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    void visit(const Symbol &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    void visit(const Integer &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    void apply(const Base &b) {
        accept(b, *this);
    }
};


template <class Derived>
static void accept(const Base &b, BaseWalkVisitor<Derived> &v) {
    std::visit(
        visitors{
            [&v](const BinOp &x) { v.visit(x); return; },
            [&v](const Pow &x) { v.visit(x); return; },
            [&v](const Symbol &x) { v.visit(x); return; },
            [&v](const Integer &x) { v.visit(x); return; },
        },
        b.u);
}

class CountVisitor : public BaseWalkVisitor<CountVisitor>
{
    int c_;
public:
    CountVisitor() : c_{0} {}
    template <typename T> void bvisit(const T &x) { }
    void bvisit(const Symbol &x) { c_ += 1; }
    int get_count() {
        return c_;
    }
};

static int count(const Base &b) {
    CountVisitor v;
    v.apply(b);
    return v.get_count();
}



#define TYPE Base*
#define ADD(x, y) al.make_new<Base>(BinOp(BinOpType::Add, x, y))
#define SUB(x, y) al.make_new<Base>(BinOp(BinOpType::Sub, x, y))
#define MUL(x, y) al.make_new<Base>(BinOp(BinOpType::Mul, x, y))
#define DIV(x, y) al.make_new<Base>(BinOp(BinOpType::Div, x, y))
#define POW(x, y) al.make_new<Base>(Pow(x, y))
#define SYMBOL(x) al.make_new<Base>(Symbol(x))
#define INTEGER(x) al.make_new<Base>(Integer(x))
//#define PRINT(x) std::cout << (long int)x << std::endl; //x->d.binop.right->type << std::endl
#define PRINT(x) std::cout << count(*x) << std::endl;
//#define PRINT(x) std::cout << count3(*x) << std::endl;


#endif
