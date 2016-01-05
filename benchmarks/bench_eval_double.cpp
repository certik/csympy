#define NONIUS_RUNNER
#include "nonius.h++"

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/constants.h>
#include <symengine/functions.h>
#include <symengine/eval_double.h>


using SymEngine::Basic;
using SymEngine::symbol;
using SymEngine::integer;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::Sin;
using SymEngine::Cos;

RCP<const Basic> get_eval_double_expression()
{
    RCP<const Basic> e = SymEngine::EulerGamma;

    for(int i = 0; i < 100000; i++) {
        e = make_rcp<const Sin>(make_rcp<const Cos>(e));
    }
    //std::cout << *e << std::endl;
    return e;
}

RCP<const Basic> e = get_eval_double_expression();

NONIUS_BENCHMARK("eval_double", [](nonius::chronometer meter) {
    double r;
    meter.measure([&](int i) {
        r = eval_double(*e);
    });
})

NONIUS_BENCHMARK("eval_double_visitor_pattern", [](nonius::chronometer meter) {
    double r;
    meter.measure([&](int i) {
        r = eval_double_visitor_pattern(*e);
    });
})

NONIUS_BENCHMARK("eval_double_single_dispatch", [](nonius::chronometer meter) {
    double r;
    meter.measure([&](int i) {
        r = eval_double_single_dispatch(*e);
    });
})
