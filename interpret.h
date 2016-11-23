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

/**
 * @file interpreter.h
 * Interpretation of instructions.
 *
 */

#ifndef INTERPRET
#define INTERPRET

#include "symbol.h"

// pseudo register
extern T_symbol *acc;

/**
 * Interprets a instruction list.
 *
 * @param run instruction list of function run
 *
 */
void interpret(T_symbol *run);

#endif
