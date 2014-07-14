#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "visitor.h"

#define ACCEPT(CLASS) void CLASS::accept(Visitor &v) const { v.visit(*this); }

namespace CSymPy {

/*
ACCEPT(Symbol)
ACCEPT(Add)
ACCEPT(Mul)
ACCEPT(Pow)
ACCEPT(Number)
ACCEPT(Function)
ACCEPT(Log)
ACCEPT(Derivative)
*/

} // CSymPy

