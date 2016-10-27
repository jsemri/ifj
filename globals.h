/*
 * globals.h
 *
 */

#ifndef GLOB_H
#define GLOB_H

#include "token.h"
#include <stdio.h>

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

// FIXME
// put here all global variables which are in use between most of modules

FILE *source;
T_token *token;

#endif
