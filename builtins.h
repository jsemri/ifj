
#ifndef BUILTINS_H
#define BUILTINS_H


#include "symbol.h"
/**
 *
 * According to result data type string from input is read and converted
 * to specific data type. Input string is unlimited. Use reallocation and
 * read input by characters.
 *
 */
void read_stdin(T_symbol *result, T_data_type dtype);


/**
 * Prints `count` items on main stack.
 *
 */
void print(int count);

void substr(T_symbol *s, T_symbol *i, T_symbol *n, T_symbol *result);

#endif
