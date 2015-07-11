#include <stdlib.h>

#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/cwrapper.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/functions.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/add.h>
#include <symengine/number.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::zero;
using SymEngine::Symbol;
using SymEngine::Rational;
using SymEngine::Integer;
using SymEngine::Number;
using SymEngine::rcp_static_cast;
using SymEngine::is_a;

// #define RCP_cast_general(x, CONST) (reinterpret_cast<CONST RCP<const Basic> *>(x))
// #define RCP_cast(x) RCP_cast_general(x, )
// #define RCP_const_cast(x) RCP_cast_general(x, const)

namespace SymEngine {

template< typename T >
inline bool is_aligned( T*p, size_t n = alignof(T) ){
    return 0 == reinterpret_cast<uintptr_t>(p) % n ;
}

}


extern "C" {

// void basic_init(basic s)
// {
// #if defined(WITH_SYMENGINE_RCP)
//     // These checks only happen at compile time.
//     // Check that 'basic' has the correct size:
//     static_assert(sizeof(RCP<const Basic>) == sizeof(basic), "Size of 'basic' is not correct");
//     // Check that 'basic' has the correct alignment:
//     static_assert(std::alignment_of<RCP<const Basic>>::value == std::alignment_of<basic>::value, "Alignment of 'basic' is not correct");
// #else
//     throw std::runtime_error("Teuchos::RCP is not compatible with the C wrappers");
// #endif
//     // No allocation is being done, but the constructor of RCP is called and
//     // the instance is initialized at the memory address 's'. The above checks
//     // make sure that 's' has the correct size and alignment, which is
//     // necessary for placement new, otherwise the results are undefined.
//     new(s) RCP<const Basic>();
// }
//
// void basic_free(basic s)
// {
//     RCP_cast(s)->~RCP();
// }

// C wrapper for SymEngine::RCP<const SymEngine::Basic>

struct CRCPBasic {
    RCP<const Basic> m;
};

CRCPBasic* basic_new()
{
    return new CRCPBasic;
}

int basic_placement_new(CRCPBasic *self, size_t size)
{
    if (!SymEngine::is_aligned(self)) return ENOTALIGNED;
    if (size < sizeof(CRCPBasic)) return EINSUFFSIZE;
    new(self) CRCPBasic;
    return 0;
}

void basic_free(CRCPBasic *self)
{
    delete self;
}

void basic_placement_free(CRCPBasic *self)
{
    self->m.~RCP();
}

// C wrapper for std::vector<int>

struct CVectorInt {
    std::vector<int> m;
};

CVectorInt* vectorint_new()
{
    return new CVectorInt;
}

int vectorint_placement_new(CVectorInt *self, size_t size)
{
    if (!SymEngine::is_aligned(self)) return ENOTALIGNED;
    if (size < sizeof(CVectorInt)) return EINSUFFSIZE;
    new(self) CVectorInt;
    return 0;
}

void vectorint_free(CVectorInt *self)
{
    delete self;
}

void vectorint_push_back(CVectorInt *self, int value)
{
    self->m.push_back(value);
}

int vectorint_get(CVectorInt *self, int n)
{
    return self->m[n];
}

// C wrapper for vec_basic

struct CVecBasic {
    SymEngine::vec_basic m;
};

CVecBasic* vecbasic_new()
{
    return new CVecBasic;
}

void vecbasic_free(CVecBasic *self)
{
    delete self;
}

void vecbasic_push_back(CVecBasic *self, const CRCPBasic *value)
{
    self->m.push_back(value->m);
}

void vecbasic_get(CVecBasic *self, int n, CRCPBasic *result)
{
    result->m = self->m[n];
}

size_t vecbasic_size(CVecBasic *self)
{
    return self->m.size();
}

// C Wrapper for set_basic

struct CSetBasic {
    SymEngine::set_basic m;
};

CSetBasic* setbasic_new()
{
    return new CSetBasic;
}

void setbasic_free(CSetBasic *self)
{
    delete self;
}

int setbasic_insert(CSetBasic *self, const CRCPBasic *value)
{
    return (self->m.insert(value->m)).second ? 1 : 0;
}

int setbasic_find(CSetBasic *self, CRCPBasic *value)
{
    return self->m.find(value->m) != (self->m).end() ? 1 : 0;
}

size_t setbasic_size(CSetBasic *self)
{
    return self->m.size();
}

// ----------------------

void basic_free_symbols(const CRCPBasic *self, CSetBasic *symbols)
{
    symbols->m = SymEngine::free_symbols(*(self->m));
}

void basic_get_args(const CRCPBasic *self, CVecBasic *args)
{
    args->m = self->m->get_args();
}

void symbol_set(CRCPBasic* self, char* c)
{
    self->m = SymEngine::symbol(std::string(c));
}

void integer_set_si(CRCPBasic* self, long i)
{
    self->m = SymEngine::integer(mpz_class(i));
}

void integer_set_ui(CRCPBasic* self, unsigned long i)
{
    self->m = SymEngine::integer(mpz_class(i));
}

void integer_set_mpz(CRCPBasic* self, const mpz_t i)
{
    self->m = SymEngine::integer(mpz_class(i));
}

void integer_set_str(CRCPBasic* self, char* c)
{
    self->m = SymEngine::integer(mpz_class(c, 10));
}

signed long integer_get_si(const CRCPBasic *self)
{
    SYMENGINE_ASSERT(is_a_Integer(self));
    return mpz_get_si((rcp_static_cast<const Integer>(self->m))->as_mpz().get_mpz_t());
}

unsigned long integer_get_ui(const CRCPBasic *self)
{
    SYMENGINE_ASSERT(is_a_Integer(self));
    return mpz_get_ui((rcp_static_cast<const Integer>(self->m))->as_mpz().get_mpz_t());
}

void integer_get_mpz(mpz_t a, const CRCPBasic *self)
{
    SYMENGINE_ASSERT(is_a_Integer(self));
    mpz_set(a, (rcp_static_cast<const Integer>(self->m))->as_mpz().get_mpz_t());
}

void rational_set_si(CRCPBasic* self, long a, long b)
{
    self->m = SymEngine::Rational::from_mpq(mpq_class(a, b));
}

void rational_set_ui(CRCPBasic* self, unsigned long a, unsigned long b)
{
    self->m = SymEngine::Rational::from_mpq(mpq_class(a, b));
}

int rational_set(CRCPBasic* self, const CRCPBasic *a, const CRCPBasic *b)
{
    if (!is_a_Integer(a) || !is_a_Integer(b)) {
        return 0;
    }
    self->m = SymEngine::Rational::from_two_ints(
            rcp_static_cast<const Integer>(a->m),
            rcp_static_cast<const Integer>(a->m));
    return 1;
}

void rational_set_mpq(CRCPBasic* self, const mpq_t i)
{
    self->m = SymEngine::Rational::from_mpq(mpq_class(i));
}

int basic_diff(CRCPBasic* result, const CRCPBasic *expr, const CRCPBasic *symbol)
{
    if (!is_a_Symbol(symbol))
        return 0;
    result->m = expr->m->diff(rcp_static_cast<const Symbol>(symbol->m));
    return 1;
}

void basic_assign(CRCPBasic* a, const CRCPBasic *b) {
    a->m = RCP<const Basic>(b->m);
}

void basic_add(CRCPBasic* result, const CRCPBasic *a, const CRCPBasic *b)
{
    result->m = SymEngine::add(a->m, b->m);
}

void basic_sub(CRCPBasic* result, const CRCPBasic *a, const CRCPBasic *b)
{
    result->m = SymEngine::sub(a->m, b->m);
}

void basic_mul(CRCPBasic* result, const CRCPBasic *a, const CRCPBasic *b)
{
    result->m = SymEngine::mul(a->m, b->m);
}

void basic_pow(CRCPBasic* result, const CRCPBasic *a, const CRCPBasic *b)
{
    result->m = SymEngine::pow(a->m, b->m);
}

void basic_div(CRCPBasic* result, const CRCPBasic *a, const CRCPBasic *b)
{
    result->m = SymEngine::div(a->m, b->m);
}

void basic_neg(CRCPBasic* result, const CRCPBasic *a)
{
    result->m = SymEngine::neg(a->m);
}

int basic_eq(const CRCPBasic *a, const CRCPBasic *b)
{
    return (SymEngine::eq(a->m, b->m)) ? 1 : 0;
}

int basic_neq(const CRCPBasic *a, const CRCPBasic *b)
{
    return (SymEngine::neq(a->m, b->m)) ? 1 : 0;
}

void basic_abs(CRCPBasic* result, const CRCPBasic *a)
{
    result->m = SymEngine::abs(a->m);
}

void basic_expand(CRCPBasic* result, const CRCPBasic *a)
{
    result->m = SymEngine::expand(a->m);
}

char* basic_str(const CRCPBasic *self)
{
    std::string str = self->m->__str__();
    char *cc = new char[str.length()+1];
    std::strcpy(cc, str.c_str());
    return cc;
}

void basic_str_free(char* s)
{
    delete[] s;
}

int is_a_Integer(const CRCPBasic *c)
{
    return is_a<Integer>(*(c->m));
}
int is_a_Rational(const CRCPBasic *c)
{
    return is_a<Rational>(*(c->m));
}
int is_a_Symbol(const CRCPBasic *c)
{
    return is_a<Symbol>(*(c->m));
}

}
