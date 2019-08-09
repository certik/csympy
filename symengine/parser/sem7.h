#ifndef SYMENGINE_PARSER_SEM7_H
#define SYMENGINE_PARSER_SEM7_H

#include <variant>

/*
   Computer 1: 12ms 139ms          127ms (linear)  195 (default new)
   Computer 2: --   78ms   92ms
*/

/*
    construction: 103ms
    total: 127ms => count: 24ms
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



static int count2(const Base &b) {
    return std::visit(
        visitors{
            [](const Symbol &x) { return 1; },
            [](const BinOp &x) {
                int c = 0;
                c += count2(*x.left);
                c += count2(*x.right);
                return c; },
            [](const Pow &x) {
                int c = 0;
                c += count2(*x.base);
                c += count2(*x.exp);
                return c; },
            [](const auto &x) { return 0; },
        },
        b.u);
}

class Visitor
{
public:
    virtual ~Visitor() {};
    virtual void visit(const BinOp &x) = 0;
    virtual void visit(const Pow &x) = 0;
    virtual void visit(const Symbol &x) = 0;
    virtual void visit(const Integer &x) = 0;
};

static void accept(const Base &b, Visitor &v) {
    return std::visit(
        visitors{
            [&v](const BinOp &x) { v.visit(x); return; },
            [&v](const Pow &x) { v.visit(x); return; },
            [&v](const Symbol &x) { v.visit(x); return; },
            [&v](const Integer &x) { v.visit(x); return; },
        },
        b.u);
}

template <class Derived>
class BaseWalkVisitor : public Visitor
{
public:
    virtual void visit(const BinOp &x) {
        apply(*x.left);
        apply(*x.right);
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    virtual void visit(const Pow &x) {
        apply(*x.base);
        apply(*x.exp);
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    virtual void visit(const Symbol &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    virtual void visit(const Integer &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    void apply(const Base &b) {
        accept(b, *this);
    }
};

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


#endif
