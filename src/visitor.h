/**
 *  \file visitor.h
 *  Class Visitor
 *
 **/
#ifndef CSYMPY_VISITOR_H
#define CSYMPY_VISITOR_H

#include "basic.h"

namespace CSymPy {

// It should be possible to simplify this using the approach described at:
// http://stackoverflow.com/a/11802080/479532
// http://stackoverflow.com/a/7877397/479532

class Visitor {
public:
    virtual ~Visitor() {};
};

template <class T>
class VisitorType {
public:
    virtual void visit(const T &) = 0;
};

class HasVisitor : public Visitor,
                   public VisitorType<Symbol>,
                   public VisitorType<Add>,
                   public VisitorType<Mul>,
                   public VisitorType<Pow>,
                   public VisitorType<Number>,
                   public VisitorType<Function>,
                   public VisitorType<Log>,
                   public VisitorType<Derivative>
{
private:
    RCP<const Symbol> x_;
    bool has_;
public:
    // TODO: allow to return true/false from the visit() methods, and if it
    // returns false, stop the traversal in pre/postorder_traversal().
    void visit(const Symbol &x) {
        if (x_->__eq__(x)) has_ = true;
    }
    void visit(const Add &) {
    }
    void visit(const Mul &) {
    }
    void visit(const Pow &) {
    }
    void visit(const Number &) {
    }
    void visit(const Function &) {
    }
    void visit(const Log &) {
    }
    void visit(const Derivative &) {
    }
    template <typename T>
    bool apply(const T &b, const RCP<const Symbol> &x) {
        x_ = x;
        has_ = false;
        static_cast<const BaseVisitable<T>&>(b).preorder_traversal(*this);
        return has_;
    }
};

template <typename T>
bool has_symbol(const T &b, const RCP<const Symbol> &x) {
    HasVisitor v;
    return v.apply(b, x);
}


} // CSymPy

#endif
