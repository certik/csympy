#include "ruby_basic.h"

static void cbasic_free(void *ptr){

}

static VALUE cbasic_alloc(VALUE self){

}

static VALUE cbasic_init(VALUE self){

}

static VALUE cbasic_release(VALUE self){

}

static VALUE cbasic_add(VALUE self, VALUE operand2){
    basic result, this, basic_operand2;
    basic_init(result);

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, basic_operand2);

    basic_add(result, this, basic_operand2);

    return Data_Make_Struct(self, basic_struct, rb_gc_mark, cbasic_release, result);//TODO: Figure out what to use cbasic_release or cbasic_free or what is the difference
}

static VALUE cbasic_sub(VALUE self, VALUE operand2){
    basic result, this, basic_operand2;
    basic_init(result);

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, basic_operand2);

    basic_sub(result, this, basic_operand2);

    return Data_Make_Struct(self, basic_struct, rb_gc_mark, cbasic_release, result);
}

static VALUE cbasic_mul(VALUE self, VALUE operand2){
    basic result, this, basic_operand2;
    basic_init(result);

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, basic_operand2);

    basic_mul(result, this, basic_operand2);

    return Data_Make_Struct(self, basic_struct, rb_gc_mark, cbasic_release, result);
}

static VALUE cbasic_div(VALUE self, VALUE operand2){
    basic result, this, basic_operand2;
    basic_init(result);

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, basic_operand2);

    basic_div(result, this, basic_operand2);

    return Data_Make_Struct(self, basic_struct, rb_gc_mark, cbasic_release, result);
}

static VALUE cbasic_pow(VALUE self, VALUE operand2){
    basic result, this, basic_operand2;
    basic_init(result);

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, basic_operand2);

    basic_pow(result, this, basic_operand2);

    return Data_Make_Struct(self, basic_struct, rb_gc_mark, cbasic_release, result);
}

static VALUE cbasic_neg(VALUE self){
    basic result;
    basic_init(result);

    Data_Get_Struct(self, basic_struct, this);

    basic_neg(result, this);

    return Data_Make_Struct(self, basic_struct, rb_gc_mark, cbasic_release, result);
}
