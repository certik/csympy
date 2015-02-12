/**
 *  \file monomials.h
 *  Monomial Multiplication
 *
 **/
 
#ifndef CSYMPY_MONOMIALS_H
#define CSYMPY_MONOMIALS_H

#include "basic.h"

namespace CSymPy {
//! Monomial multiplication
void monomial_mul(const vec_int4 &A, const vec_int4 &B, vec_int4 &C);

} // CSymPy

#endif
