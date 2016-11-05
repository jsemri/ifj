#ifndef IAL_H
#define IAL_H

#include <stdbool.h>
#include "instruction.h"
#include "string.h"

// symbol
typedef struct T_symbol {
    // key and identifier
    const char *id;
    // symbol type
    enum {
        isclass,
        isfunc,
        isvar
    } symbol_type;
    // data type (including return value)
    enum {
        isstr,
        isint,
        isdouble
    } date_type;
    // `defined` flag
    bool is_def;
    // possible values
    union {
        T_string *str;
        int num;
        double d;
    } value;
    // symbol attribute
    union {
        bool is_init;        // initialization flag of
        ilist *func_ilist;   // function's instruction list
    } symbol_attr;
    // member class of variable or function
    struct T_symbol *member_class;
    // next symbol
    struct T_symbol *next;
} T_symbol;

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
void table_remove(T_symbol_table *stab);

// deletes one item from the table (called by table_remove)
void symbol_remove(T_symbol_table *stab, const char *key);

#endif
