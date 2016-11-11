/**
 * @file ial.h
 * @brief Contains symbol table.
 */

#ifndef IAL_H
#define IAL_H

#include <stdbool.h>
#include "instruction.h"
#include "string.h"


struct T_Hash_symbol_table;

// data types
typedef enum {
    is_void,
    is_int,
    is_double,
    is_str
} T_data_types;

/// Function attributes.
typedef struct {
    ilist *func_ilist;          // instruction list
    unsigned par_count;         // number of parameters
    void **arguments;           // pointer to parameters in local table
    unsigned local_count;                       // local variable count
    struct T_Hash_symbol_table *local_table;    // local symbol table
} T_func_symbol;

/// Variable attributes.
typedef struct {
    union {
        T_string *str;
        int num;
        double d;
    } value;                         // value of variable
} T_var_symbol;


/// Structure of symbol.
typedef struct T_symbol {
    const char *id;                 // symbol identifier, first key
    enum {
        is_class,
        is_func,
        is_var
    } symbol_type;                  // symbol type
    T_data_types data_type;         // symbol data type
    union {
        T_var_symbol *var;
        T_func_symbol *func;
    } attr;                         // symbol attribute
    struct T_symbol *member_class;  // member class, second key
    struct T_symbol *next;          // next symbol in list
} T_symbol;

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
 * @brief search for symbol with `key` and member class `mclass`
 *
 * @param *stab pointer to symbol table
 * @param key identifier name
 * @param *mclass pointer to a member class
 * @return Pointer to the searched symbol, NULL if symbol was not found.
 */
T_symbol *table_find(T_symbol_table *stab, const char *key, T_symbol *mclass);

/**
 * @brief Inserts symbol item to symbol table.
 *
 * @param *stab pointer to symbol table
 * @param s symbol to insert
 * @return Pointer to the inserted symbol.
 */
T_symbol *table_insert(T_symbol_table *stab, T_symbol *s);

/**
 * @brief Deletes whole table and sets it's pointer to NULL.
 *
 * @param *stab pointer to symbol table
 */
void table_remove(T_symbol_table **stab);

#endif
