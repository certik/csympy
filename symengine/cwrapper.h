#ifndef CWRAPPER_H
#define CWRAPPER_H

#include <stdlib.h>
#include <gmp.h>

//! Error codes
//! Better name suggestions needed
#define ENOTALIGNED 2
#define EINSUFFSIZE 1

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// The size of 'basic_struct' must be the same as RCP<const Basic> *and* they
// must have the same alignment (because we allocate RCP<const Basic> into the
// memory occupied by this struct in cwrapper.cpp). We cannot use C++ in this
// file, so we need to use C tools to arrive at the correct size and alignment.
// The size of the RCP object on most platforms (with WITH_SYMENGINE_RCP on)
// should be just the size of the 'T *ptr_' pointer that it contains (as there
// is no virtual function table) and the alignment should also be of a pointer.
// So we just put 'void *data' as the only member of the struct, that should
// have the correct size and alignment. With WITH_SYMENGINE_RCP off (i.e. using
// Teuchos::RCP), we have to add additional members into the structure.
//
// However, this is checked at compile time in cwrapper.cpp, so if the size or
// alignment is different on some platform, the compilation will fail --- in
// that case one needs to modify the contents of this struct to adjust its size
// and/or alignment.
typedef struct
{
    void *data;
#if !defined(WITH_SYMENGINE_RCP)
    void *teuchos_handle;
    int teuchos_strength;
#endif
} basic_struct;

//! 'basic' is internally implemented as a size 1 array of the type
//   basic_struct, which has the same size and alignment as RCP<const Basic>
//   (see the above comment for details). That is then used by the user to
//   allocate the memory needed for RCP<const Basic> on the stack. A 'basic'
//   type should be initialized using basic_init(), before any function is
//   called.  Assignment should be done only by using basic_assign(). Before
//   the variable goes out of scope, basic_free() must be called.
typedef basic_struct basic[1];


// //! Initialize a new basic instance.
// void basic_init(basic s);
// //! Assign value of b to a.
// void basic_assign(basic a, const basic b);
// //! Free the C++ class wrapped by s.
// void basic_free(basic s);

//! Wrapper for SymEngine::RCP<const Basic>

typedef struct CRCPBasic CRCPBasic;

CRCPBasic* basic_new();
int basic_placement_new(CRCPBasic *self, size_t size);
void basic_free(CRCPBasic *self);
void basic_placement_free(CRCPBasic *self);

//! Assign to s, a symbol with string representation c.
void symbol_set(CRCPBasic* self, char* c);

//! Assign to s, a long.
void integer_set_si(CRCPBasic* self, long i);
//! Assign to s, a ulong.
void integer_set_ui(CRCPBasic* self, unsigned long i);
//! Assign to s, a mpz_t.
void integer_set_mpz(CRCPBasic* self, const mpz_t i);
//! Assign to s, an integer that has base 10 representation c.
void integer_set_str(CRCPBasic* self, char* c);

//! Returns signed long value of s.
signed long integer_get_si(const CRCPBasic* self);
//! Returns unsigned long value of s.
unsigned long integer_get_ui(const CRCPBasic* self);
//! Returns s as a mpz_t.
void integer_get_mpz(mpz_t a, const CRCPBasic* self);

//! Assign to s, a rational i/j. Returns 0 if either i or j is not an integer.
int rational_set(CRCPBasic* self, const CRCPBasic* i, const CRCPBasic* j);
//! Assign to s, a rational i/j, where i and j are signed longs.
void rational_set_si(CRCPBasic* self, long i, long j);
//! Assign to s, a rational i/j, where i and j are unsigned longs.
void rational_set_ui(CRCPBasic* self, unsigned long i, unsigned long j);
//! Assign to s, a rational i, where is of type mpq_t.
void rational_set_mpq(CRCPBasic* self, const mpq_t i);

//! Assigns s = a + b.
void basic_add(CRCPBasic* result, const CRCPBasic* a, const CRCPBasic* b);
//! Assigns s = a - b.
void basic_sub(CRCPBasic* result, const CRCPBasic* a, const CRCPBasic* b);
//! Assigns s = a * b.
void basic_mul(CRCPBasic* result, const CRCPBasic* a, const CRCPBasic* b);
//! Assigns s = a / b.
void basic_div(CRCPBasic* result, const CRCPBasic* a, const CRCPBasic* b);
//! Assigns s = a ** b.
void basic_pow(CRCPBasic* result, const CRCPBasic* a, const CRCPBasic* b);
//! Assign to s, derivative of expr with respect to sym. Returns 0 if sym is not a symbol.
int basic_diff(CRCPBasic* result, const CRCPBasic* expr, const CRCPBasic* sym);
//! Assigns s = -a.
void basic_neg(CRCPBasic* result, const CRCPBasic* a);
//! Returns 1 if both basic are equal, 0 if not
int basic_eq(const CRCPBasic* a, const CRCPBasic* b);
//! Returns 1 if both basic are not equal, 0 if they are
int basic_neq(const CRCPBasic* a, const CRCPBasic* b);
//! Assigns s = abs(a).
void basic_abs(CRCPBasic* result, const CRCPBasic* a);
//! Expands the expr a and assigns to s.
void basic_expand(CRCPBasic* result, const CRCPBasic* a);

//! Returns a new char pointer to the string representation of s.
char* basic_str(const CRCPBasic* self);
//! Frees the string s
void basic_str_free(char* s);

//! Return 1 if s is an Integer, 0 if not.
int is_a_Integer(const CRCPBasic *self);
//! Return 1 if s is an Rational, 0 if not.
int is_a_Rational(const CRCPBasic *self);
//! Return 1 if s is an Symbol, 0 if not.
int is_a_Symbol(const CRCPBasic *self);


//! Wrapper for std::vector<int>

typedef struct CVectorInt CVectorInt;

CVectorInt* vectorint_new();

// 'self' must point to allocated memory of size 'size'. The function returns 0
// if std::vector<int> can be initialized using placement new into 'self',
// otherwise 1 if 'size' is too small or 2 if 'self' is not properly aligned.
// No memory is leaked either way.
int vectorint_placement_new(CVectorInt *self, size_t size);

void vectorint_free(CVectorInt *self);
void vectorint_push_back(CVectorInt *self, int value);
int vectorint_get(CVectorInt *self, int n);

//! Wrapper for vec_basic

typedef struct CVecBasic CVecBasic;

CVecBasic* vecbasic_new();
void vecbasic_free(CVecBasic *self);
void vecbasic_push_back(CVecBasic *self, const CRCPBasic *value);
void vecbasic_get(CVecBasic *self, int n, CRCPBasic *result);
size_t vecbasic_size(CVecBasic *self);

//! Wrapper for set_basic

typedef struct CSetBasic CSetBasic;
CSetBasic* setbasic_new();
void setbasic_free(CSetBasic *self);
//! Returns 1 if insert is successful and 0 if set already contains the value
//! and insertion is unsuccessful
int setbasic_insert(CSetBasic *self, const CRCPBasic *value);
//! Returns 1 if value is found in the set and 0 if not
int setbasic_find(CSetBasic *self, CRCPBasic *value);
size_t setbasic_size(CSetBasic *self);
// -------------------------------------

//! Returns a CSetBasic of set_basic given by free_symbols
void basic_free_symbols(const CRCPBasic *self, CSetBasic *symbols);
//! Returns a CVecBasic of vec_basic given by get_args
void basic_get_args(const CRCPBasic *self, CVecBasic *args);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //CWRAPPER_H
