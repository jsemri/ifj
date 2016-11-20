/**
 * @file alg.h
 * @brief Contains symbol table.
 */

#ifndef ALG_H
#define ALG_H

#include <stdbool.h>
#include "instruction.h"
#include "string.h"
#include "symbol.h"

enum heapsort {
    HEAPSORT_OK,
    HEAPSORT_FAIL,
};

int heap_sort(T_string *src, T_string *dest);

/**
 * \brief Function for heapsort, reeastablishment of heap.
 * @param   string   Pointer to string.
 * @param   left     Index of the most left node.
 * @param   right    Index of the most righ node.
 */
void sift_down(T_string *string, int left, int right);

/**
 * \brief Function finds substring in string using Knuth-Morris-Pratt algorithm
 *		  and returns its position.
 *        If substring is not found, function returns -1.
 *
 * @param   string      Pointer to string.
 * @param   pattern     Pointer to substring.
 *
 * @return  Index of first occurrence substring in string.
 */

int find_kmp(T_string *string, T_string *pattern);

#endif