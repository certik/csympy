#ifndef SYMENGINE_PARSER_SEM4_H
#define SYMENGINE_PARSER_SEM4_H

#include "alloc.h"
#include "symengine/symengine_casts.h"

namespace SymEngine {

extern Allocator al;

}

using SymEngine::al;

// Computer 1: 12ms 128ms
// Computer 2: 13ms 74ms   83ms

/*

construction: 97ms
total: 118ms => count: 21ms

-----------
gcc 7.4.0
BinOp refactoring:

construction: 105ms
total: 118ms => count: 13ms

gcc 9.1.0
construction: 117ms
total: 132ms => count 15ms
C++2a
construction: 116ms
total: 126ms => count 10ms
*/
enum NodeType
{
    nt_BinOp, nt_Pow, nt_Symbol, nt_Integer
};

enum BinOpType
{
    Add, Sub, Mul, Div
};

typedef struct Node *PNode;

struct BinOp {
    BinOpType type;
    PNode left, right;
};

struct Pow {
    PNode base, exp;
};

struct Symbol {
    char *name;
};

struct Integer {
    char *i;
};

struct Node {
    NodeType type;
    union {
        BinOp binop;
        Pow pow;
        Symbol symbol;
        Integer integer;
    } d;
};


static Node* make_binop(BinOpType type, PNode x, PNode y) {
    PNode n;
    n = al.make_new<Node>();
    n->type = NodeType::nt_BinOp;
    n->d.binop.type = type;
    n->d.binop.left = x;
    n->d.binop.right = y;
    return n;
}

static Node* make_pow(PNode x, PNode y) {
    PNode n;
    n = al.make_new<Node>();
    n->type = NodeType::nt_Pow;
    n->d.pow.base = x;
    n->d.pow.exp = y;
    return n;
}

static Node* make_symbol(std::string s) {
    PNode n;
    n = al.make_new<Node>();
    n->type = NodeType::nt_Symbol;
    n->d.symbol.name = &s[0];
    return n;
}

static Node* make_integer(std::string s) {
    PNode n;
    n = al.make_new<Node>();
    n->type = NodeType::nt_Integer;
    n->d.integer.i = &s[0];
    return n;
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

static void accept(const Node &x, Visitor &v) {
    switch (x.type) {
        case nt_BinOp: { v.visit(x.d.binop); return; }
        case nt_Pow: { v.visit(x.d.pow); return; }
        case nt_Symbol: { v.visit(x.d.symbol); return; }
        case nt_Integer: { v.visit(x.d.integer); return; }
    }
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
    void apply(const Node &b) {
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

static int count(const Node &b) {
    CountVisitor v;
    v.apply(b);
    return v.get_count();
}


#define TYPE PNode
#define ADD(x, y) make_binop(BinOpType::Add, x, y)
#define SUB(x, y) make_binop(BinOpType::Sub, x, y)
#define MUL(x, y) make_binop(BinOpType::Mul, x, y)
#define DIV(x, y) make_binop(BinOpType::Div, x, y)
#define POW(x, y) make_pow(x, y)
#define SYMBOL(x) make_symbol(x)
#define INTEGER(x) make_integer(x)
//#define PRINT(x) std::cout << x->d.binop.right->type << std::endl
#define PRINT(x) std::cout << count(*x) << std::endl;


#endif
