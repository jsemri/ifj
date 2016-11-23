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
 * @file builtins.h
 * Implementation of built-in functions.
 *
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "symbol.h"

/**
 * Reads from standard input.
 *
 * @param result where input will be stored
 * @param dtype which data should be read
 *
 */
void read_stdin(T_symbol *result, T_data_type dtype);


/**
 * Prints items in main stack.
 *
 * @param count  number of items in stack to be printed
 *
 */
void print(int count);


/**
 * Creates a substring from given string.
 *
 * @param sym1 string model
 * @param sym2 starting index
 * @param syn3 length of substring
 * @result new substring
 */
void substr(T_symbol *sym1, T_symbol *sym2, T_symbol *sym3, T_symbol *result);

/**
 * Compare two strings.
 *
 * @param sym1 first string
 * @param sym2 second string
 * @param result result of comparsion
 */
void compare(T_symbol *sym1, T_symbol *sym2, T_symbol *result);

/**
 * Length of string.
 *
 * @param sym1 string
 * @param result length of string
 */
void length(T_symbol *sym1, T_symbol *result);

void clear_buffer(T_symbol *sym);

/**
 * Concatanation of strings or numbers. Result is always string.
 *
 * @param sym1 whatever
 * @param sym2 whateer
 * @param result result of concatanation
 */
void concat(T_symbol *sym1, T_symbol *sym2, T_symbol *result);

/**
 * Sorting a string by heap sort.
 *
 * @param sym1 source string
 * @param result sorted string
 */
void sort(T_symbol *sym, T_symbol *result);

/**
 * Finds a substring in string.
 *
 * @param sym1 source string
*  @param sym2 substring
 * @param result index of first occurence of sym2 in sym1
 */
void find(T_symbol *sym1, T_symbol *sym2, T_symbol *result);

#endif
