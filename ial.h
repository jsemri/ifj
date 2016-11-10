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

// function attributes
typedef struct {
    // instruction list
    ilist *func_ilist;
    // parameter count
    unsigned par_count;
    // pointer to parameters in local table
    void **arguments;
    // local variable count
    unsigned local_count;
    // local symbol table
    struct T_Hash_symbol_table *local_table;
} T_func_symbol;

// variable attributes
typedef struct {
    // value of variable
    union {
        T_string *str;
        int num;
        double d;
    } value;
} T_var_symbol;


// symbol
typedef struct T_symbol {
    // key and identifier
    const char *id;
    // symbol type
    enum {
        is_class,
        is_func,
        is_var
    } symbol_type;
    // data type (including return value)
    T_data_types data_type;
    // symbol attributes
    union {
        T_var_symbol *var;
        T_func_symbol *func;
    } attr;
    // member class
    struct T_symbol *member_class;
    // next symbol
    struct T_symbol *next;
} T_symbol;

typedef struct T_Hash_symbol_table {
    unsigned size;      // hash table size
    T_symbol *arr[];    // array of lists
} T_symbol_table;

// hash function
unsigned hash(const char *key, unsigned size);

// initialization of symbol table
T_symbol_table *table_init(unsigned size);

// search for symbol with `key` and member class `mclass`
// returns a pointer to the searched symbol
// returns NULL if symbol was not found
T_symbol *table_find(T_symbol_table *stab, const char *key, T_symbol *mclass);

// inserts symbol item to symbol table
// returns pointer to the inserted symbol
T_symbol *table_insert(T_symbol_table *stab, T_symbol *s);

// deletes whole table
void table_remove(T_symbol_table **stab);

#endif
