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

// function attributes
typedef struct {
    // instruction list
    // NULL if no function body
    ilist *func_ilist;
    // number of parameters
    unsigned par_count;
    // data types of parameters
    // NULL if no parameters
    T_data_types *dtypes;
    // parameters names
    // NULL if no parameters
    char **par_names;
} T_func_symbol;

// variable attributes
typedef struct {
    // initialization flag
    bool is_init;
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
        isclass,
        isfunc,
        isvar
    } symbol_type;
    /*
       Using static variable or function before declaration.
       If undefined variable or function is found (or class), it is added to
       symbol table with flag set to 0. After leaving a member class. If the
       flag is unchanged semantic error have occurred.
    */
    bool is_def;

    // data type (including return value)
    T_data_types data_type;
    // symbol attributes
    union {
        T_var_symbol *var;
        T_func_symbol *func;
    } attr;
    // member class of variable or function
    struct T_symbol *member_class;
    // next symbol
    struct T_symbol *next;
} T_symbol;

#endif
