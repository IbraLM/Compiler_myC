%{

#include "src/Table_des_symboles.h"
#include "src/Attribute.h"
#include "src/Production_de_code.h"

#include <stdio.h>
#include <string.h>
extern int yylex();
extern int yyparse();
extern int yylineno;

void yyerror (char* s) {
  printf ("\e[1;31m %s\e[0m at line %d\n",s,yylineno);
  exit(-1);
  }

%}

%union { 
	struct ATTRIBUTE * val;
}
%token <val> NUMI NUMF
%token TINT TFLOAT//  STRUCT
%token <val> ID
%token AO AF PO PF PV VIR
%token RETURN VOID EQ
%token <val> IF ELSE WHILE

%token <val> AND OR NOT DIFF EQUAL SUP INF
%token PLUS MOINS STAR DIV
%token DOT ARR

%type <val> type typename vlist pointer aff exp fun fun_head fun_body params app arglist bool_cond cond inst else elsop while_cond loop while args ret

%left DIFF EQUAL SUP INF       // low priority on comparison
%left PLUS MOINS               // higher priority on + - 
%left STAR DIV                 // higher priority on * /
%left OR                       // higher priority on ||
%left AND                      // higher priority on &&
%left DOT ARR                  // higher priority on . and -> 
%nonassoc UNA                  // highest priority on unary operator
%nonassoc ELSE

%start prog  
 


%%

prog : func_list               {}
;

func_list : func_list fun      {}
| fun                          {}
;


// I. Functions

fun : type fun_head fun_body        {produce_code($2,$2,  fun);}
;

fun_head : ID PO PF            {$$ = $1;
				$$->type_val = $<val>0->type_val;
				$$->pointer_counter = $<val>0->pointer_counter;
				set_symbol_value($$->name,$$);
				produce_code($$,$$,  fun_head);}
				
| ID PO params PF              {$$ = $1;
				$$->type_val = $<val>0->type_val;
				$$->pointer_counter = $<val>0->pointer_counter;
				set_symbol_value($$->name,$$);
				$$->right_arg = $3;
				$$->arg_number = $3->arg_number + 1;
                                produce_code($$,$$,  fun_head);
                                produce_code($$,$$,  params);
                                }

;

params: type ID vir params     {$$ = $2;
				$$->type_val = $1->type_val;
				$$->pointer_counter = $1->pointer_counter;
				$$->reg_number = new_reg($$);
				set_symbol_value($$->name,$$);
				$$->arg_number = $4->arg_number + 1;
				$$->right_arg = $4;
				}

| type ID                      {$$ = $2;
				$$->type_val = $1->type_val;
				$$->pointer_counter = $1->pointer_counter;
				$$->reg_number = new_reg($$);
				set_symbol_value($$->name,$$);
				$$->arg_number = 1;
				}
;
vlist: vlist vir ID            {$$ = $3;
				$$->type_val = $<val>0->type_val;
				$$->pointer_counter = $<val>0->pointer_counter;
				set_symbol_value($$->name,$$);
				legit_decl($$); //return an error message if the declaration is not valid
				produce_code($$, $$, vlist_2_id);
				}
				
| ID                           {$$ = $1;
				$$->type_val = $<val>0->type_val;
				$$->pointer_counter = $<val>0->pointer_counter;
				set_symbol_value($$->name,$$);
				legit_decl($$); //return an error message if the declaration is not valid
				produce_code($$, $$, vlist_2_id);
				}
;

vir : VIR                      {}
;

fun_body : AO block AF         {produce_code($$, $$, fun_body);}
;

// Block
block: decl_list inst_list           {}
;

// I. Declarations

decl_list : decl_list decl     {}
|                              {}
;

decl: var_decl PV              {}
;

var_decl : type vlist          {}
;

type
: typename pointer             {$$ = $1;
				$$->pointer_counter = $2->pointer_counter;}
				
| typename                     {}
;

typename
: TINT                         	{$$ = new_attribute();
				 $$->type_val = INT;}
				 
| VOID                          {$$ = new_attribute();
				 $$->type_val = VVOID;}
				 
| TFLOAT                          {$$ = new_attribute();
				 $$->type_val = FLOAT;}
				
;

pointer
: pointer STAR                 {$$ = $1;
				$$->pointer_counter++;}
				
| STAR                         {$$ = new_attribute();
				$$->pointer_counter++;}
;


// II. Intructions

inst_list: inst PV inst_list {}
| inst pvo                   {}
;

pvo : PV
|
;


inst:
exp                           {}
| AO block AF                 {}
| aff                         {}
| ret                         {}
| cond                        {}
| loop                        {}
| PV                          {}
;


// II.1 Affectations

aff : ID EQ exp               {$$ = get_symbol_value($1->name);
				legit_affect($$, $3);
				produce_code($$, $3, aff_2_id_eq_exp); }
				
| STAR exp  EQ exp		{$$ = $2;
				produce_code($$, $4, aff_2_star_exp);}
;


// II.2 Return
ret : RETURN exp              {$$ = $2;
				produce_code($$, $$, ret_exp);}
| RETURN PO PF                {$$ = new_attribute();
				produce_code($$, $$, ret);}
;

// II.3. Conditionelles
//           N.B. ces rêgles génèrent un conflit déclage reduction qui est résolu comme on le souhaite
//           i.e. en lisant un ELSE en entrée, si on peut faire une reduction elsop, on la fait...

cond :
if bool_cond inst elsop 	{produce_code($2, $4, cond_2_if_bool_inst);}
;

elsop : else inst             {}
				
|				{$$ = new_attribute();
                                 $$->label = $<val>-1->label_end;}
;

bool_cond : PO exp PF         {$$ = $2;
				$$->label = new_label();
				$$->label_if = new_label();
				$$->label_end = new_label();
				produce_code($$, $$, bool_cond);}
;

if : IF                       {}
;

else : ELSE                   {$$ = new_attribute();
				$$->label = new_label();
				$$->label_end = $<val>-1->label_end;
				produce_code($$, $$, else_2_else);}
;

// II.4. Iterations

loop : while while_cond inst  {produce_code($1, $2, loop);}
;

while_cond : PO exp PF        {$$ = $2;
				//$$->label = new_label();
				$$->label_end = new_label();
				produce_code($$, $$, while_cond);}

while : WHILE                 {$$ = new_attribute();
				$$->label = new_label();
				produce_code($$, $$, while_2_while);}
;


// II.3 Expressions

// II.3.0 Exp. arithmetiques
exp : MOINS exp %prec UNA         {$$ = neg_attribute($2);
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $2, $2, negation);}
				
| exp PLUS exp                {$$ = plus_attribute($1,$3);
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3, addition);}
				
| exp MOINS exp               {$$ = minus_attribute($1,$3);
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3, substraction);}
				
| exp STAR exp                {$$ = mult_attribute($1,$3);
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3, multiplication);}
				
| exp DIV exp                 {$$ = div_attribute($1,$3);
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3, division);}
				
| PO exp PF                   {$$ = $2;}

| ID                          {$$ = get_symbol_value($1->name);
				$$->reg_number = new_reg($$);
				produce_code($$, $$,  exp_2_id);}
				
| app                         {}

| NUMI                        {$$ = $1;
				$$->reg_number = new_reg($$);
				produce_code($$, $$,  exp_2_numi);}
| NUMF                        {$$ = $1;
				$$->reg_number = new_reg($$);
				produce_code($$, $$,  exp_2_numf);}

// II.3.1 Déréférencement

| STAR exp %prec UNA          {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				$$->type_val = $2->type_val;
				$$->pointer_counter = $2->pointer_counter - 1;
				produce_code($$, $2,  deref);}

// II.3.2. Booléens

| NOT exp %prec UNA           {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $2, $2,  not);}
				
| exp INF exp                 {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3,  inf);}
				
| exp SUP exp                 {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3,  sup);}
				
| exp EQUAL exp               {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3,  equal);}
				
| exp DIFF exp                {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3, different);}
				
| exp AND exp                 {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3, and);}
				
| exp OR exp                  {$$ = new_attribute();
				$$->reg_number = new_reg($$);
				produce_expression_code($$, $1, $3, or);}

;

// II.4 Applications de fonctions

app : ID PO args PF		{$$ = $1;
				get_symbol_value($$->name);
				$$->reg_number = new_reg($$);
				$$->arg_counter = $3->arg_counter;
				produce_code($$, $$, app);}
;

args :  arglist               {}
|                             {}
;

arglist : exp VIR arglist     {$$ = $1;
				$$->arg_counter = $3->arg_counter + 1;
				$$->arg_number = $3->arg_number + 1;
				produce_code($$, $$, arglist);}

| exp                         {$$ = $1;
				$$->arg_counter = 1;
				$$->arg_number = 1;
				produce_code($$, $$, arglist_2_exp);}
;



%% 
int main () {
	FILE* produced_code_c = fopen("./test/test.c", "w");
	FILE* produced_code_h = fopen("./test/test.h", "w");

	init_produced_code(produced_code_c, produced_code_h);
	yyparse ();
	create_main();


	fclose(produced_code_c);
	fclose(produced_code_h);
	return 0;
} 

