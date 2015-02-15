/**
 *  \file rings.h
 *  Polynomial Manipulation
 *
 **/
#ifndef CSYMPY_RINGS_H
#define CSYMPY_RINGS_H

#include "basic.h"
#include "dict.h"

namespace CSymPy {

typedef std::array<int, 4> vec_int4;

//! Part of umap_vec_mpz:
typedef struct
{
    inline std::size_t operator() (const vec_int4 &k) const {
        std::size_t h = 0;
        for (auto &p: k) {
            h = (h << 4) + p;
        }
        return h;
    }
} vec_int_hash;

typedef struct
{
    //! \return true if `x==y`
    inline bool operator() (const vec_int4 &x, const vec_int4 &y) const {
        return x == y;
    }
} vec_int_eq;

using my_int = std::int64_t;

typedef std::unordered_map<vec_int4, my_int,
        vec_int_hash, vec_int_eq> umap_vec_mpz;


//! Converts expression `p` into a polynomial `P`, with symbols `sym`
void expr2poly(const RCP<const Basic> &p, umap_basic_num &syms,
        umap_vec_mpz &P);

//! Multiply two polynomials: `C = A*B`
void poly_mul(const umap_vec_mpz &A, const umap_vec_mpz &B, umap_vec_mpz &C);
void poly_print_stats(const umap_vec_mpz &A);

} // CSymPy

#endif
