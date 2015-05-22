#include <ruby.h>
#include "cwrapper.h"

static void cbasic_free(void *ptr);

static VALUE cbasic_alloc(VALUE self);

static VALUE cbasic_init(VALUE self);

static VALUE cbasic_release(VALUE self);

static VALUE cbasic_add(VALUE self, VALUE operand2);

static VALUE cbasic_sub(VALUE self, VALUE operand2);

static VALUE cbasic_mul(VALUE self, VALUE operand2);

static VALUE cbasic_div(VALUE self, VALUE operand2);

static VALUE cbasic_pow(VALUE self, VALUE operand2);

static VALUE cbasic_neg(VALUE self);
