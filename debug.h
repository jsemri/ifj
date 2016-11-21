#ifndef DEBUG_H
#define DEBUG_H

#include "ial.h"
#include "instruction.h"

void print_instr(T_instr *ins);
void print_function(T_symbol *func);
void print_table(T_symbol_table *st);
int show_token(int rc);
int leave(const char *, int);
void enter(const char *);
void interpret_error(int ec);
void print_error(int ec);
#endif
