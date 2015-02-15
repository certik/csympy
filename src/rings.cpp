#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "pow.h"
#include "symbol.h"
#include "rings.h"
#include "monomials.h"

namespace CSymPy {

void expr2poly(const RCP<const Basic> &p, umap_basic_num &syms, umap_vec_mpz &P)
{
    if (is_a<Add>(*p)) {
        //int n = syms.size();
        const RCP<const Number> add_coeff =
            rcp_static_cast<const Add>(p)->coef_;
        const umap_basic_num &d = rcp_static_cast<const Add>(p)->dict_;
        vec_int4 exp;
        exp.fill(0);
        if (!is_a<Integer>(*add_coeff))
                throw std::runtime_error("Not implemented.");
        P[exp] = rcp_static_cast<const Integer>(add_coeff)->as_int();
        mpz_class coef;
        for (auto &p: d) {
            if (!is_a<Integer>(*p.second))
                    throw std::runtime_error("Not implemented.");
            coef = rcp_static_cast<const Integer>(p.second)->as_mpz();
            exp.fill(0); // Initialize to [0]*n
            if (is_a<Mul>(*p.first)) {
                const map_basic_basic &term = rcp_static_cast<const Mul>(p.first)->dict_;
                for (auto &q: term) {
                    RCP<const Basic> sym = q.first;
                    if (!is_a<Integer>(*syms.at(sym)))
                            throw std::runtime_error("Not implemented.");
                    int i = rcp_static_cast<const Integer>(syms.at(sym))->as_int();
                    if (is_a<Integer>(*q.second)) {
                        exp[i] = rcp_static_cast<const Integer>(q.second)->as_int();
                    } else {
                        throw std::runtime_error("Cannot convert symbolic exponents to sparse polynomials with integer exponents.");
                    }
                }
            } else if (is_a<Pow>(*p.first)) {
                RCP<const Basic> sym = rcp_static_cast<const Pow>(p.first)->base_;
                RCP<const Basic> exp_ = rcp_static_cast<const Pow>(p.first)->exp_;
                if (!is_a<Integer>(*syms.at(sym)))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<const Integer>(syms.at(sym))->as_int();
                if (!is_a<Integer>(*exp_))
                    throw std::runtime_error("Not implemented.");
                exp[i] = rcp_static_cast<const Integer>(exp_)->as_int();
            } else if (is_a<Symbol>(*p.first)) {
                RCP<const Basic> sym = p.first;
                if (!is_a<Integer>(*syms.at(sym)))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<const Integer>(syms.at(sym))->as_int();
                exp[i] = 1;
            } else {
                throw std::runtime_error("Not implemented.");
            }

            //std::string tmp = coef.get_str();
            //P[exp] = flint::fmpzxx{tmp.c_str()};
            if (!(coef.fits_sint_p())) {
                throw std::runtime_error("as_int: Integer larger than int");
            }
            P[exp] = coef.get_si();
        }
    } else {
        throw std::runtime_error("Not implemented.");
    }
}

void poly_mul(const umap_vec_mpz &A, const umap_vec_mpz &B, umap_vec_mpz &C)
{
    vec_int4 exp;
//    int n = (A.begin()->first).size();
    exp.fill(0); // Initialize to [0]*n
    /*
    std::cout << "A: " << A.load_factor() << " " << A.bucket_count() << " " << A.size() << " "
        << A.max_bucket_count() << std::endl;
    std::cout << "B: " << B.load_factor() << " " << B.bucket_count() << " " << B.size() << " "
        << B.max_bucket_count() << std::endl;
    std::cout << "C: " << C.load_factor() << " " << C.bucket_count() << " " << C.size() << " "
        << C.max_bucket_count() << std::endl;
        */
    for (auto &a: A) {
        for (auto &b: B) {
            monomial_mul(a.first, b.first, exp);
            C[exp] += a.second*b.second;
        }
    }
    /*
    std::cout << "C: " << C.load_factor() << " " << C.bucket_count() << " " << C.size() << " "
        << C.max_bucket_count() << std::endl;
    for (std::size_t n=0; n < C.bucket_count(); n++) {
        std::cout << n << ": " << C.bucket_size(n) << "|";
        for (auto it = C.begin(n); it != C.end(n); ++it)
            std::cout << " " << it->first << myhash2(it->first) % C.bucket_count();
        std::cout << std::endl;
    }
    */
}

typedef __mpz_struct mpz_t2[3];

void poly_print_stats(const umap_vec_mpz &A)
{
    my_int min=std::numeric_limits<my_int>::max();
    my_int max=std::numeric_limits<my_int>::min();
    for (auto &a: A) {
        if (a.second < min) min = a.second;
        if (a.second > max) max = a.second;
    }
    std::cout << "min: " << min << std::endl;
    std::cout << "max: " << max << std::endl;
    std::cout << "size mpz_class:    " << sizeof(mpz_class) << std::endl;
    std::cout << "size mpz_t:        " << sizeof(mpz_t) << std::endl;
    std::cout << "size mpz_t2:       " << sizeof(mpz_t2) << std::endl;
    std::cout << "size __mpz_struct: " << sizeof(__mpz_struct) << std::endl;
    std::cout << "size my_int:       " << sizeof(my_int) << std::endl;
}

} // CSymPy
