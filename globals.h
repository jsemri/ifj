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

// FIXME Put here all global variables which are used across most of the modules

extern T_stack *frame_stack;   // local tables will be copied here
extern T_stack *main_stack;
extern T_symbol_table *symbol_tab;
extern FILE *source;
extern T_token *token;
// used when token vector cannot be freed
extern token_vector global_token_vector;

struct T_pool {
    void **first;
    void **last;
    int size;
};

void *alloc(size_t size);
void *ralloc(void *ptr, size_t size);
void terminate(int err_code);

#endif
