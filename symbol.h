#ifndef SYMBOL_H
#define SYMBOL_H

#include "token.h"
#include "ilist.h"
#include "token_vector.h"
#include <stdbool.h>

struct T_Hash_symbol_table;

typedef enum {
    is_class,
    is_func,
    is_var
} T_symbol_type;

// data types
typedef enum {
    is_void = 0,
    is_int = 1,
    is_double = 2,
    is_str = 3,
    is_bool = 4
} T_data_type;

/// Function attributes.
/// Variable attributes.
typedef struct {
    T_data_type data_type;          // data type
    bool is_init;
    bool is_const;                  // 0 if constant
    T_value value;                  // value of variable
} T_var_symbol;

/// Function attributes.
typedef struct {
    T_data_type data_type;      // return data type
    T_var_symbol return_var;    // return variable
    ilist *func_ilist;          // instruction list
    unsigned par_count;         // number of parameters
    void **arguments;           // pointer to parameters in local table
    unsigned local_count;                       // local variable count
    struct T_Hash_symbol_table *local_table;    // local symbol table
} T_func_symbol;

/// Structure of symbol.
typedef struct T_symbol {
    char *id;                // symbol identifier, first key
    T_symbol_type symbol_type;     // symbol type
    union {
        T_var_symbol *var;
        T_func_symbol *func;
    } attr;                         // symbol attribute
    struct T_symbol *member_class;  // member class, second key
    struct T_symbol *next;          // next symbol in list
} T_symbol;

/**
 * @brief Creates a symbol without variable or function attributes.
 *
 * @param iden symbol identifier
 * @param stype symbol type - class, variable or function
 * @return valid pointer on success
 */
T_symbol *create_symbol(char *id, T_symbol_type stype);

/**
 * @brief Creates a symbol - variable.
 *
 * @param id Name of the variable
 * @param dtype data type
 * @return valid pointer on success
 */
T_symbol *create_var(char *id, T_data_type dtype);

/**
 * @brief Creates a symbol - variable with an arbitrary name.
 *
 * @param dtype data type
 * @return valid pointer on success
 */
T_symbol *create_var_uniq(T_data_type dtype);

void func_remove(T_symbol *sym);
void var_remove(T_symbol *sym);

/**
 * @brief Creates a function.
 *
 * @param dtype data type
 * @return valid pointer on success
 */
T_symbol *create_func(char *id, T_data_type dtype);

/**
 * Searches for variable (also full identifier is possible) with specific
 * name and data type. Just for type and definition control.
 *
 * @param iden variable identifier
 * @param local_tab local table of function, body of actual function
 * @param actual_class body of actual class
 * @param dtype data type
 * @return pointer to symbol
 */
T_symbol *is_defined(char *iden, struct T_Hash_symbol_table *local_tab,
             T_symbol *actual_class, T_data_type dtype);

/**
 * Creates a variable, a constant, and inserts it to symbol table.
 * Variable name is also it's value. In attempt to insert existing
 * constant of same data type, pointer to existing constant is returned.
 *
 * @param value variable identifier and it's value
 * @param global symbol table
 * @param dtype data type
 * @return pointer to constant variable
 */
T_symbol *add_constant(T_value *value, struct T_Hash_symbol_table*,
                       T_data_type dtype);

/**
 * @brief Deletes whole table and sets it's pointer to NULL.
 *
 * @param loc_stab pointer to symbol table
 */
void local_table_remove(struct T_Hash_symbol_table **loc_stab);

T_symbol *symbol_copy(T_symbol *sym);

int fill_ifj16();

char *get_str(const char *src);

#endif
