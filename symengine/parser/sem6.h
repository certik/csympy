#ifndef SYMENGINE_PARSER_SEM6_H
#define SYMENGINE_PARSER_SEM6_H

/*
   Computer 1: 12ms 122ms           111ms (linear)  187ms (default new)
   Computer 2: -    74ms   86ms
*/

/*
gcc 7.4.0
   99ms construction
   111ms with count => count: 12ms

gcc 9.1.0
    construction: 112ms
    total: 128ms => count: 16ms
    C++2a
    construction: 111ms
    total: 127ms => count: 16ms
   */

/*
   gcc 7.4.0 110ms
   Clang-6 104ms
   */

#include "alloc.h"
#include "symengine/symengine_casts.h"

namespace SymEngine {

extern Allocator al;

}

using SymEngine::al;

template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


enum BinOpType
{
    Add, Sub, Mul, Div
};

class Base;
class Visitor;
class BinOp;
class Pow;
class Symbol;
class Integer;
static int count(const Base &b);

class Visitor
{
public:
    virtual ~Visitor() {};
    virtual void visit(const BinOp &x) = 0;
    virtual void visit(const Pow &x) = 0;
    virtual void visit(const Symbol &x) = 0;
    virtual void visit(const Integer &x) = 0;
};

class Base {
public:
    virtual ~Base() {};
    virtual void accept(Visitor &v) const = 0;
};

class BinOp : public Base {
public:
    const BinOpType type;
    const Base *left, *right;
    BinOp(BinOpType type, const Base *x, const Base *y)
        : type{type}, left{x}, right{y} {
    }
    virtual ~BinOp() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};

class Pow : public Base {
public:
    const Base *base, *exp;
    Pow(const Base *x, const Base *y)
        : base{x}, exp{y} {
    }
    virtual ~Pow() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};

class Symbol : public Base {
public:
    const char *name;
    Symbol(const std::string s) : name{s.c_str()} { }
    virtual ~Symbol() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};

class Integer : public Base {
    const char *i;
public:
    Integer(const std::string s) : i{s.c_str()} { }
    virtual ~Integer() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};


template <class Derived>
class BaseVisitor : public Visitor
{
public:
    virtual void visit(const BinOp &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    virtual void visit(const Pow &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    virtual void visit(const Symbol &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    virtual void visit(const Integer &x) {
        SymEngine::down_cast<Derived *>(this)->bvisit(x);
    }
    void apply(const Base &b) {
        b.accept(*this);
    }
};

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
        b.accept(*this);
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
#define ADD(x, y) al.make_new<BinOp>(BinOpType::Add, x, y)
#define SUB(x, y) al.make_new<BinOp>(BinOpType::Sub, x, y)
#define MUL(x, y) al.make_new<BinOp>(BinOpType::Mul, x, y)
#define DIV(x, y) al.make_new<BinOp>(BinOpType::Div, x, y)
#define POW(x, y) al.make_new<Pow>(x, y)
#define SYMBOL(x) al.make_new<Symbol>(x)
#define INTEGER(x) al.make_new<Integer>(x)
#define PRINT(x) std::cout << count(*x) << std::endl;
//#define PRINT(x) std::cout << (long int)(x) << std::endl;


#endif
