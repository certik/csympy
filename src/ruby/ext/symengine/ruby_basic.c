#include "ruby_basic.h"

void cbasic_free(void *ptr){
    basic_struct *basic_ptr = ptr;
    basic_free(basic_ptr);
}

VALUE cbasic_alloc(VALUE klass){
    VALUE obj;
    basic_struct *struct_ptr;

    obj = Data_Make_Struct(klass, basic_struct, NULL, cbasic_free, struct_ptr);

    struct_ptr->data = NULL;

    return obj;
}

VALUE cbasic_init(VALUE self){
    basic_struct *this;

    Data_Get_Struct(self, basic_struct, this);
    basic_init(this);

    return self;
}

VALUE cbasic_add(VALUE self, VALUE operand2){
    basic_struct *this, *cbasic_operand2, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL , cbasic_free, cresult);

    basic_init(cresult);
    basic_add(cresult, this, cbasic_operand2);

    return result;
}

VALUE cbasic_sub(VALUE self, VALUE operand2){
    basic_struct *this, *cbasic_operand2, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL , cbasic_free, cresult);

    basic_init(cresult);
    basic_sub(cresult, this, cbasic_operand2);

    return result;
}

VALUE cbasic_mul(VALUE self, VALUE operand2){
    basic_struct *this, *cbasic_operand2, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL , cbasic_free, cresult);

    basic_init(cresult);
    basic_mul(cresult, this, cbasic_operand2);

    return result;
}

VALUE cbasic_div(VALUE self, VALUE operand2){
    basic_struct *this, *cbasic_operand2, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL , cbasic_free, cresult);

    basic_init(cresult);
    basic_div(cresult, this, cbasic_operand2);

    return result;
}

VALUE cbasic_pow(VALUE self, VALUE operand2){
    basic_struct *this, *cbasic_operand2, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL , cbasic_free, cresult);

    basic_init(cresult);
    basic_pow(cresult, this, cbasic_operand2);

    return result;
}

VALUE cbasic_neg(VALUE self){
    basic_struct *this, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL, cbasic_free, cresult);

    basic_init(cresult);
    basic_neg(cresult, this);

    return result;
}
