/*
 * IFJ 2016
 * FIT VUT Brno
 * IFJ16 Interpret Project
 *
 * Authors:
 * Jakub   Semric     - xsemri00
 * Peter   Rusinak    - xrusin03
 * Krystof Rykala     - xrykal00
 * Martin  Mikan      - xmikan00
 * Martin  Polakovic  - xpolak33
 *
 * Unless otherwise stated, all code is licenced under a
 * GNU General Public License v2.0
 *
 */

/*
 * globals.h
 *
 */

#ifndef GLOB_H
#define GLOB_H

#include "token.h"
#include "token_vector.h"
#include <stdio.h>
#include "ial.h"
#include "instruction.h"
#include "stack.h"
#include "symbol.h"
#include "ilist.h"

// error codes
#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define DEFINITION_ERROR 3
#define TYPE_ERROR 4
#define SEMANTIC_ERROR 6
#define INPUT_ERROR 7
#define INIT_ERROR 8
#define DEVZERO_ERROR 9
#define OTHER_ERROR 10
#define INTERNAL_ERROR 99

// symbol table array length
#define RANGE 8


extern T_stack *frame_stack;   // local tables will be copied here
extern T_stack *main_stack;
extern T_symbol_table *symbol_tab;
extern ilist *glist;
extern FILE *source;
extern T_token *token;
// used when token vector cannot be freed
extern token_vector global_token_vector;

// for debug
extern int row;
extern int part;
extern const char *fun;
extern unsigned gins;

// for lexer
extern char *char_vector;
extern int vector_size;

// for dividing merged lists
extern T_instr *div_point;

struct T_pool {
    void **first;
    void **last;
    int size;
};

void *alloc(size_t size);
void *ralloc(void *ptr, size_t size);
void terminate(int err_code);

#endif
