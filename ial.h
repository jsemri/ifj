/**
 * @file ial.h
 * @brief Contains symbol table.
 */

#ifndef IAL_H
#define IAL_H

#include <stdbool.h>
#include "instruction.h"
#include "string.h"
#include "symbol.h"

typedef struct T_Hash_symbol_table {
    unsigned size;      // hash table size
    T_symbol *arr[];    // array of lists
} T_symbol_table;

unsigned hash(const char *key, unsigned size);

/**
 * @brief Initializes symbol table.
 *
 * @param size length of list array
 * @return Pointer to new symbol table, NULL if allocation fails.
 */
T_symbol_table *table_init(unsigned size);

/**
 * Search for symbol with `key` and member class `mclass`, also full
 * identifier possible.
 *
 * @param stab pointer to symbol table
 * @param key identifier name
 * @param mclass pointer to a member class
 * @return Pointer to the searched symbol, NULL if symbol was not found.
 */
T_symbol *table_find(T_symbol_table *stab, char *key, T_symbol *mclass);

T_symbol *table_find_simple(T_symbol_table *stab, char *key, T_symbol *mclass);

/**
 * @brief Inserts symbol item to symbol table.
 *
 * @param stab pointer to symbol table
 * @param s symbol to insert
 * @return Pointer to the inserted symbol.
 */
T_symbol *table_insert(T_symbol_table *stab, T_symbol *s);

/**
 * @brief Deletes whole table and sets it's pointer to NULL.
 *
 * @param stab pointer to symbol table
 */
void table_remove(T_symbol_table **stab);

/**
 * \brief Functions sorts strings.
 *
 * @param   str    Pointer to string.
 *
 */
void heap_sort(char *str);

/**
 * \brief Function for heapsort, reeastablishment of heap.
 * @param   string   Pointer to string.
 * @param   left     Index of the most left node.
 * @param   right    Index of the most righ node.
 */
void sift_down(char *str, int left, int right);

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
int find_kmp(char *str, char *pattern);

#endif
