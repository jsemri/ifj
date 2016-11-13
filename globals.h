/*
 * globals.h
 *
 */

#ifndef GLOB_H
#define GLOB_H

#include "token.h"
#include <stdio.h>
#include "ial.h"
#include "instruction.h"
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

extern ilist *instr_list;
extern T_symbol_table *symbol_tab;
extern FILE *source;
extern T_token *token;

void terminate(int err_code);

#endif
