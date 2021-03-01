#include "Attribute.h"

#include <stdlib.h>
#include <stdio.h>

#define MAX_REG 200

extern int yylineno;

attribute new_attribute() {
    attribute r;
    r = malloc(sizeof(struct ATTRIBUTE));
    r->pointer_counter = 0;
    r->arg_counter = 0;
    return r;
};


attribute plus_attribute(attribute x, attribute y) {
    if (x->type_val != y->type_val) {
        fprintf(stderr,"Error : you try to add two different type of value\n");
        exit(-1);
    }
    attribute r = new_attribute();
    r->type_val = x->type_val;
    return r;
};

attribute mult_attribute(attribute x, attribute y) {
    if (x->type_val != y->type_val) {
        fprintf(stderr,"Error : you try to multipliate two different type of value");
        exit(-1);
    }
    attribute r = new_attribute();
    r->type_val = x->type_val;
    return r;
};

attribute minus_attribute(attribute x, attribute y) {
    if (x->type_val != y->type_val) {
        fprintf(stderr,"Error : you try to substrate two different type of value");
        exit(-1);
    }
    attribute r = new_attribute();
    r->type_val = x->type_val;
    return r;
};

attribute div_attribute(attribute x, attribute y) {
    if (x->type_val != y->type_val) {
        fprintf(stderr,"Error : you try to divide two different type of value");
        exit(-1);
    }
    attribute r = new_attribute();
    r->type_val = x->type_val;
    return r;
};

attribute neg_attribute(attribute x) {
    attribute r = new_attribute();
    r->type_val = x->type_val;
    return r;
};

char* get_type(type t) {
    switch (t) {
        case INT:
            return "int";
            break;
        case FLOAT:
            return "float";
            break;
        case VVOID:
            return "void";
            break;
    }
}

/*
 * if a register i is free and have never been used, reg[i-1] = 0
 * if a register i is not free, reg[i-1] = 1
 * if a register i is free but have already be used, reg[i-1] = 1
 */
int reg[MAX_REG] = {0};

/*
 * reg_type[i] represent the type of the register i+1
 */
type reg_type[MAX_REG] = {0};

/*
 * reg_pointer[i] represent the pointer counter of the register i+1
 */
int reg_pointer[MAX_REG] = {0};

/*
 * return a number of register that was free and of the right type if the register was already defined
 */
int new_reg(attribute a){
    for(int i = 0; i < MAX_REG; i++) {
        if (reg[i] == 0) {
            reg[i] = 1;
            reg_type[i] = a->type_val;
            reg_pointer[i] = a->pointer_counter;
            return i + 1;
        }
        if (reg[i] == 2
        && reg_type[i] == a->type_val
        && reg_pointer[i] == a->pointer_counter) {
            reg[i] = 1;
            return i + 1;
        }
    }
}

/*
 * free the register i
 * at that point, do nothing
 */
void free_reg(int i){
    (void) i;
    //reg[i - 1] = 2;
}

int new_label() {
    static int l = 0;
    return ++l;
}

//is a declaration (int a;) legit
void legit_decl(attribute a) {
    if (a->type_val == VVOID) {
        fprintf(stderr,"\e[1;31m error\e[0m at line %d : you can't declare a void variable\n", yylineno);
        exit(-1);
    }
}

//is an affectation (id = exp) legit
void legit_affect(attribute id, attribute exp){
    if (id->pointer_counter != exp->pointer_counter) {
        fprintf(stderr,"\e[1;31m error\e[0m at line %d : you try to affect two different pointer type\n", yylineno);
        exit(-1);
    }
    if (id->type_val != exp->type_val) {
        fprintf(stderr,"\e[1;31m error\e[0m at line %d : you try to affect a %s value to a %s variable\n", yylineno, get_type(exp->type_val), get_type(id->type_val));
        exit(-1);
    }
}