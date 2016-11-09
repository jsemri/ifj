#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>
#include "instruction.h"
#include "string.h"

// data types
typedef enum {
    isstr,
    isint,
    isdouble,
    isvoid
} T_data_types;

// parameters of function
typedef struct {
    unsigned par_count;
    char *names[];  // argument names
    T_data_types *dtypes;
} T_params;


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
    T_data_types data_type;
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
    T_params *params;         // function's parameters
    // member class of variable or function
    struct T_symbol *member_class;
    // next symbol
    struct T_symbol *next;
} T_symbol;

#endif
