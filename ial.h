#ifndef IAL_H
#define IAL_H

#include "symbol.h"

typedef struct {
    unsigned size;      // hash table size
    T_symbol *arr[];    // array of lists
} T_symbol_table;

// hash function
unsigned hash(const char *key, unsigned size);

// initialization of symbol table
T_symbol_table *table_init(unsigned size);

// search for symbol with `key`
// returns a pointer to the searched symbol
// returns NULL if symbol was not found
T_symbol *table_find(T_symbol_table *stab, const char *key);

// inserts symbol item to symbol table
// returns pointer to the inserted symbol
T_symbol *table_insert(T_symbol_table *stab, T_symbol *s);

// deletes whole table
void table_remove(T_symbol_table **stab);

#endif
