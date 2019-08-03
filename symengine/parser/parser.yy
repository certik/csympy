%require "3.0"

/*
TODO:
    * hook the tokenizer in
    * fix the shift/reduce conflict
*/

%code requires // *.h
{
#include "symengine/add.h"
#include "symengine/pow.h"
#include "symengine/logic.h"
#include "symengine/parser/parser_new.h"

// TODO: move these to cpp
using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::vec_basic;
using SymEngine::rcp_static_cast;
using SymEngine::mul;
using SymEngine::pow;
using SymEngine::add;
using SymEngine::sub;
using SymEngine::Lt;
using SymEngine::Gt;
using SymEngine::Le;
using SymEngine::Ge;
using SymEngine::Eq;
using SymEngine::set_boolean;
using SymEngine::Boolean;
using SymEngine::one;
using SymEngine::vec_boolean;

namespace yy {
    extern SymEngine::Parser2 p;
}

}

%code // *.cpp
{

#include "symengine/parser/tokenizer.h"


namespace yy
{

SymEngine::Parser2 p;

parser::symbol_type yylex ()
{
    return p.d_tokenizer->lex();
} // ylex

void parser::error(const std::string &msg)
{
    throw SymEngine::ParseError(msg + "XXX");
}

} // namespace yy

} // code


%token <std::string> IDENTIFIER
%token <std::string> NUMERIC
%token <std::string> IMPLICIT_MUL
%token END_OF_FILE 0

%left '|'
%left '^'
%left '&'
%left EQ
%left '>'
%left '<'
%left LE
%left GE
%left '-' '+'
%left '*' '/'
%right UMINUS
%right POW
%right NOT
%nonassoc '('

%type <RCP<const Basic>> st_expr
%type <RCP<const Basic>> expr
%type <vec_basic> expr_list
%type <RCP<const Basic>> leaf
%type <RCP<const Basic>> func

%start st_expr

%%
st_expr :
    expr
    {
        $$ = $1;
        p.res = $$;
    }
;

expr:
        expr '+' expr
        { $$ = add($1, $3); }
|
        expr '-' expr
        { $$ = sub($1, $3); }
|
        expr '*' expr
        { $$ = mul($1, $3); }
|
        expr '/' expr
        { $$ = div($1, $3); }
|
// FIXME: This rule generates:
// parser.yy: warning: 1 shift/reduce conflict [-Wconflicts-sr]
        IMPLICIT_MUL POW expr
        {
          auto tup = p.parse_implicit_mul($1);
          if (neq(*std::get<1>(tup), *one)) {
            $$ = mul(std::get<0>(tup), pow(std::get<1>(tup), $3));
          } else {
            $$ = pow(std::get<0>(tup), $3);
          }
        }
|
        expr POW expr
        { $$ = pow($1, $3); }
|
        expr '<' expr
        { $$ = rcp_static_cast<const Basic>(Lt($1, $3)); }
|
        expr '>' expr
        { $$ = rcp_static_cast<const Basic>(Gt($1, $3)); }
|
        expr LE expr
        { $$ = rcp_static_cast<const Basic>(Le($1, $3)); }
|
        expr GE expr
        { $$ = rcp_static_cast<const Basic>(Ge($1, $3)); }
|
        expr EQ expr
        { $$ = rcp_static_cast<const Basic>(Eq($1, $3)); }
|
        expr '|' expr
        { 
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>($1));
            s.insert(rcp_static_cast<const Boolean>($3));
            $$ = rcp_static_cast<const Basic>(logical_or(s));
        }
|
        expr '&' expr
        { 
            set_boolean s;
            s.insert(rcp_static_cast<const Boolean>($1));
            s.insert(rcp_static_cast<const Boolean>($3));
            $$ = rcp_static_cast<const Basic>(logical_and(s));
        }
|
        expr '^' expr
        {
            vec_boolean s;
            s.push_back(rcp_static_cast<const Boolean>($1));
            s.push_back(rcp_static_cast<const Boolean>($3));
            $$ = rcp_static_cast<const Basic>(logical_xor(s));
        }
|
        '(' expr ')'
        { $$ = $2; }
|
        '-' expr %prec UMINUS
        { $$ = neg($2); }
|
        '~' expr %prec NOT
        { $$ = rcp_static_cast<const Basic>(logical_not(rcp_static_cast<const Boolean>($2))); }
|
        leaf
        { $$ = rcp_static_cast<const Basic>($1); }
;

leaf:
    IDENTIFIER
    {
        $$ = p.parse_identifier($1);
    }
|
    IMPLICIT_MUL
    {
        auto tup = p.parse_implicit_mul($1);
        $$ = mul(std::get<0>(tup), std::get<1>(tup));
    }
|
    NUMERIC
    {
        $$ = p.parse_numeric($1);
    }
|
    func
    {
        $$ = $1;
    }
;

func:
    IDENTIFIER '(' expr_list ')'
    {
        $$ = p.functionify($1, $3);
    }
;

expr_list:

    expr_list ',' expr
    {
        $$ = $1; // TODO : should make copy?
        $$ .push_back($3);
    }
|
    expr
    {
        $$ = vec_basic(1, $1);
    }
;
