#include <stdio.h>
#include <stdlib.h>

#include "Attribute.h"

#ifndef COMPILATION_11459_PRODUCTION_DE_CODE_H
#define COMPILATION_11459_PRODUCTION_DE_CODE_H

enum symbole {
    vlist_2_id,
    aff_2_id_eq_exp,
    aff_2_star_exp,
    exp_2_id,
    exp_2_numi,
    exp_2_numf,
    fun_head,
    fun_body,
    params,
    app,
    arglist,
    deref,
    cond_2_if_bool_inst,
    bool_cond,
    else_2_else,
    while_cond,
    loop,
    while_2_while,
    fun,
    arglist_2_exp,
    ret,
    ret_exp
};

enum operation {
    addition,
    substraction,
    multiplication,
    division,
    negation,
    not,
    inf,
    sup,
    equal,
    different,
    and,
    or
};

void produce_code(attribute a, attribute b, enum symbole symbole);
void produce_expression_code(attribute a, attribute b, attribute c, enum operation operation);
void create_main();
void init_produced_code(FILE* f_c, FILE* f_h);


#endif //COMPILATION_11459_PRODUCTION_DE_CODE_H
