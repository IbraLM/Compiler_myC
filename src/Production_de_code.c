#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Attribute.h"
#include "Production_de_code.h"

static FILE* file_c;
static FILE* file_h;

void fprintf_star(FILE* file, int n) {
    for (int i = 0; i < n; i++)
        fprintf(file, "*");
}

void declare_reg(FILE* f, attribute a) {
    fprintf(f, "%s ", get_type(a->type_val));
    fprintf_star(f, a->pointer_counter);
    fprintf(f, "r%d; \n", a->reg_number);
}

void produce_code(attribute a, attribute b, enum symbole symbole ) {
    switch (symbole) {
            //declaration of variable (int* a;)
        case vlist_2_id:
            fprintf(file_c, "\t%s ", get_type(a->type_val));
            fprintf_star(file_c, a->pointer_counter);
            fprintf(file_c, "%s;\n", a->name);
            break;

            // memory write (a = r1)
        case aff_2_id_eq_exp:
            fprintf(file_c, "\t%s = r%d; \n", a->name, b->reg_number);
            free_reg(b->reg_number);
            break;

            // memory write with pointer(*r1 = r2)
        case aff_2_star_exp:
            fprintf(file_c, "\t*r%d = r%d; \n", a->reg_number, b->reg_number);
            free_reg(b->reg_number);
            break;

            //memory read var (r1 = a)
        case exp_2_id:
            declare_reg(file_h, a);
            fprintf(file_c, "\tr%d = %s;\n ", a->reg_number, a->name);
            break;

            //memory read with pointer (r1 = *r2)
        case deref:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = *r%d;\n", a->reg_number, b->reg_number);
            break;

            //r1 = 2
        case exp_2_numi:
            declare_reg(file_h, a);
            fprintf(file_c, "\tr%d = %d;\n", a->reg_number, a->int_val);
            break;

            //r1 = 2.2
        case exp_2_numf:
            declare_reg(file_h, a);
            fprintf(file_c, "\tr%d = %f;\n", a->reg_number, a->float_val);
            break;

            /*
             * begin to define function (call_f() {... )
             * create a printf that give the name of the function when the executable is executed
             */
        case fun_head:
            fprintf(file_c, "void call_%s() {\n", a->name);
            fprintf(file_c, "\tprintf(\"\\t%s()\\n\");\n", a->name);
            break;

            //end of defining function (... } )
        case fun_body:
            fprintf(file_c, "end_function: ;\n");
            fprintf(file_c, "}\n\n");
            break;

            /*
             * initialize the arguments of the function with the call stack
             * (r1 = GET_ARGUMENT(1); int a; a = r1;)
             * create a printf that give the arguments of the function when the executable is executed
             */
        case params:
            while (a->arg_number > 1){
                a = a->right_arg;
                declare_reg(file_h, a);
                fprintf(file_c, "\tr%d = GET_ARGUMENT(%d);\n", a->reg_number, a->arg_number);
                fprintf(file_c, "\t%s ", get_type(a->type_val));
                fprintf_star(file_c, a->pointer_counter);
                fprintf(file_c, "%s;\n", a->name);
                fprintf(file_c, "\t%s = r%d;\n", a->name, a->reg_number);
                free_reg(a->reg_number);
                fprintf(file_c, "\tprintf(\"\\t%s = %s\",%s);\n", a->name, "%d\\n", a->name);
            }
            break;

            /*
             * set the return value in the stack ( SET_RETURN(r1); )
             * create a printf that give the return of the function when the executable is executed
             * go to the end of the function
             */
        case ret_exp:
            fprintf(file_c,"\tSET_RETURN(r%d);\n", a->reg_number);
            fprintf(file_c, "\tprintf(\"\\treturn-> %s \",r%d);\n", "%d\\n", a->reg_number);
            fprintf(file_c,"\tgoto end_function;\n");
            break;

            /*
             * go to the end of the function
             */
        case ret:
            fprintf(file_c,"\tgoto end_function;\n");
            break;

            /*
             * call a function ( call_f(); )
             * get the returning value in the call stack ( r1 = GET_RETURN_VALUE(); )
             * pop the arguments and return value from the call stack ( POP() )
             * if the function don't have arguments, initialize the call stack before ( INIT_STACK() )
             */
        case app:
            if(a->arg_counter == 0)
                fprintf(file_c, "\tINIT_STACK();\n");
            fprintf(file_c, "\tcall_%s();\n",a->name);
            declare_reg(file_h, a);
            fprintf(file_c, "\tr%d = GET_RETURN_VALUE();\n", a->reg_number);
            fprintf(file_c, "\tPOP();\n");
            break;

            //push an argument on the call stack ( PUSH(r1) )
        case arglist:
            fprintf(file_c, "\tPUSH(r%d);\n", a->reg_number);
            break;

            /*
             * initialize the call stack ( INIT_STACK() )
             * push an argument on the stack ( PUSH(r1) )
             */
        case arglist_2_exp:
            fprintf(file_c, "\tINIT_STACK();\n");
            fprintf(file_c, "\tPUSH(r%d);\n", a->reg_number);
            break;

            /*
             * create the begining of the if then else structure
             * Exemple:
             *      goto label_if;
             * l1:
             *      ...
             */
        case bool_cond:
            fprintf(file_c,"\tgoto l%d;\n", a->label_if);
            fprintf(file_c,"l%d:\n", a->label);
            break;

            /*
            * create the middle of the if then else structure
            * Exemple:
            *      ...
            *      goto label_end;
            * l2:
            *      ...
            */
        case else_2_else:
            fprintf(file_c,"\tgoto l%d;\n", a->label_end);
            fprintf(file_c,"l%d:\n", a->label);
            break;

            /*
             * create the end of the if then else structure
             * Exemple:
             *      ...
             *      goto label_end:
             * label_if:
             *      if (r1) goto l1
             *      if (!r1) goto l2
             * label_end:
             */
        case cond_2_if_bool_inst:
            declare_reg(file_h, a);
            fprintf(file_c,"\tgoto l%d;\n", a->label_end);
            fprintf(file_c,"l%d:\n", a->label_if);
            fprintf(file_c,"\tif (r%d) goto l%d;\n", a->reg_number, a->label);
            fprintf(file_c,"\tif (!r%d) goto l%d;\n", a->reg_number, b->label);
            fprintf(file_c,"l%d: ;\n", a->label_end);
            break;

            /*
             * create the begining of a loop structure
             *      if (r1) goto label_end;
             *      ...
             */
        case while_cond:
            fprintf(file_c, "\tif (r%d) goto l%d;\n", a->reg_number, a->label_end);
            break;

            /* create the middle of a loop structure
             *      ...
             * l1:
             *      ...
             */
        case while_2_while:
            fprintf(file_c, "l%d:\n", a->label);
            break;

            /*
             * create the end of a loop structure
             *      ...
             *      goto l1
             * label_end:
             */
        case loop:
            fprintf(file_c, "\tgoto l%d;\n", a->label);
            fprintf(file_c, "l%d:\n\t;\n", b->label_end);
            break;
    }
}

void produce_expression_code(attribute a, attribute b, attribute c, enum operation operation) {
    switch (operation) {
            //r3 = r1 + r2
        case addition:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d + r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 - r2
        case substraction:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d - r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 * r2
        case multiplication:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d * r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 / r2
        case division:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d / r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = -r2
        case negation:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = -r%d;\n", a->reg_number, b->reg_number);
            free_reg(b->reg_number);
            break;
            //r3 = !r1
        case not:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = !r%d;\n", a->reg_number, b->reg_number);
            free_reg(b->reg_number);
            break;
            //r3 = r1 < r2
        case inf:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d < r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 > r2
        case sup:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d > r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 == r2
        case equal:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d == r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 != r2
        case different:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d != r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 && r2
        case and:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d && r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
            //r3 = r1 || r2
        case or:
            declare_reg(file_h, a);
            fprintf(file_c,"\tr%d = r%d || r%d;\n", a->reg_number, b->reg_number, c->reg_number);
            free_reg(b->reg_number);
            free_reg(c->reg_number);
            break;
    }
}

// create a main that call the main function defined in the .myc file
void create_main() {
    fprintf(file_c, "int main() {\n\tcall_main(); \n\treturn 0; \n}\n\n ");
}

/*
 * initialize the file_c and file_h
 * create the include in file_c
 * define the call stack and its functions at the begining of file_c
 */
void init_produced_code(FILE* f_c, FILE* f_h) {
    file_c = f_c;
    file_h = f_h;
    fprintf(file_c, "#include <stdio.h> \n#include <stdlib.h> \n\n#include \"test.h\" \n\n");
    fprintf(file_c, "/*********** Define the call stack and its functions ************/\n");
    fprintf(file_c, "#define STACK_SIZE 30\n");
    fprintf(file_c, "int stack[STACK_SIZE];\n");
    fprintf(file_c, "int* sp = stack;\n");
    fprintf(file_c, "int* fp = stack;\n");
    fprintf(file_c, "void PUSH(int r) {*sp = r; sp = sp + 1;}\n");
    fprintf(file_c, "int POP() {sp = fp - 1; fp = sp;}\n");
    fprintf(file_c, "int GET_ARGUMENT(int arg_number) {return *(fp + arg_number);}\n");
    fprintf(file_c, "void SET_RETURN(int return_value) {*fp = return_value;}\n");
    fprintf(file_c, "int GET_RETURN_VALUE() {return *fp;}\n");
    fprintf(file_c, "void INIT_STACK() {*sp = (long) fp; sp++; fp = sp; sp++;}\n\n");
}