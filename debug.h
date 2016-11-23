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
