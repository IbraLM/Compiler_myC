/*
 *  Attribute.h
 *
 *  Created by Janin on 10/2019
 *  Copyright 2018 LaBRI. All rights reserved.
 *
 *  Module for a clean handling of attibutes values
 *
 */

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/* Represents the possible types */
typedef enum {INT, FLOAT, VVOID} type; // We add a VOID type

/* Represents an attribut */
struct ATTRIBUTE {
  char * name;
  int int_val;
  float float_val;
  type type_val;
  int reg_number;

  /*
    Represents the number of references
    Example : 
    int: pointer_counter = 0
    int**: pointer_counter = 2 
  */
  int pointer_counter;

  /*
    Represent the labels for conditional branching
    Example:
    ...
        goto label_if;
    label 1:
        ...
        goto label_end;
    label 2:
        ...
        goto label_end;
    label_if:
        if (bool) goto label 1;
        if (!bool) goto label 2;
    label_end:
        ...
  */
  int label_if;
  int label_end;
  int label;

  /* Represent the number of argument of function*/
  int arg_counter;

  /* Represent the place of an argument in a function signature*/
  int arg_number;

  /* Represent the argument on the right of the argument attribute */
  struct ATTRIBUTE*  right_arg;

};

typedef struct ATTRIBUTE * attribute;

/*translate the type t as a string*/
char* get_type(type t);

/* returns the pointeur to a newly allocated (but uninitialized) attribute value structure */
attribute new_attribute ();

attribute plus_attribute(attribute x, attribute y);
attribute mult_attribute(attribute x, attribute y);
attribute minus_attribute(attribute x, attribute y);
attribute div_attribute(attribute x, attribute y);
attribute neg_attribute(attribute x);

/* create a new number of register*/
int new_reg(attribute a);

void free_reg(int i);

/* create a new number of label*/
int new_label();

void legit_decl(attribute a);
void legit_affect(attribute id, attribute exp);

#endif

