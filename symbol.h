#ifndef SYMBOL_H
#define SYMBOL_H

#include "token.h"
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
    is_void,
    is_int,
    is_double,
    is_str
} T_data_type;

/// Function attributes.
/// Variable attributes.
typedef struct {
    T_data_type data_type;          // data type
    bool is_initialized;
    bool is_const;                  // 0 if constant
    union {
        T_string *str;
        int num;
        double d;
    } value;                        // value of variable
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
    const char *id;                // symbol identifier, first key
    T_symbol_type symbol_type;     // symbol type
    T_data_type data_type;         // symbol data type
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
 * @brief Creates a variable.
 *
 * @param dtype data type
 * @return valid pointer on success
 */
T_var_symbol *create_var(T_data_type dtype);

/**
 * @brief Creates a function.
 *
 * @param dtype data type
 * @return valid pointer on success
 */
T_func_symbol *create_func(T_data_type dtype);

/**
 * @brief Search for variable with specific name and data type.
 *
 * @param iden variable identifier
 * @param local_tab local table of function, body of actual class
 * @param actual_class body of actual class
 * @param dtype data type
 * @return 0 in success or specific error
 */
int find_var(const char *iden, struct T_Hash_symbol_table *local_tab,
             T_symbol *actual_class, T_data_type dtype);
/**
 * @brief Copies symbol table and puts it on stack.
 *
 * @param sym_tab source symbol table
 * @return pointer to new symbol table
 */
struct T_Hash_symbol_table *table_copy(const struct T_Hash_symbol_table *sym_tab);

/**
 * @brief Deletes whole table and sets it's pointer to NULL.
 *
 * @param loc_stab pointer to symbol table
 */
void local_table_remove(struct T_Hash_symbol_table **loc_stab);

int fill_ifj16();
void remove_ifj16();

#endif
