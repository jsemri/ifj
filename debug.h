#ifndef DEBUG_H
#define DEBUG_H

#include "ial.h"

void print_function(T_symbol *func);
void print_table(T_symbol_table *st);
int show_token(int rc);
int leave(const char *, int);
void enter(const char *);
#endif
