#include "globals.h"
#include "symbol.h"
#include "ial.h"
#include "token.h"
#include "token_vector.h"
#include "scanner.h"
#include "string.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

T_symbol_table *symbol_tab;

T_func_symbol *create_func(T_data_type dtype)
{{{

    T_func_symbol *func = calloc(1, sizeof(T_func_symbol));

    if (!func) {
        terminate(INTERNAL_ERROR);
    }
    func->data_type = dtype;

    if (!(func->local_table = table_init(RANGE))) {
        free(func);
        terminate(INTERNAL_ERROR);
    }

    return func;
}}}

T_symbol *create_symbol(char *id, T_symbol_type stype)
{{{

    T_symbol *sym = calloc(1, sizeof(T_symbol));

    if (!sym) {
        terminate(INTERNAL_ERROR);
    }
    sym->symbol_type = stype;
    sym->id = id;

    return sym;
}}}

T_var_symbol *create_var_from_symbol(T_data_type dtype)
{{{

    T_var_symbol *var = calloc(1, sizeof(T_var_symbol));

    if (!var) {
        terminate(INTERNAL_ERROR);
    }
    var->data_type = dtype;

    return var;
}}}

T_symbol *create_var(char *id, T_data_type dtype)
{{{
    T_symbol *sym = create_symbol(id, is_var);
    sym->attr.var = create_var_from_symbol(dtype);
    return sym;
}}}


int is_defined(char *iden, T_symbol_table *local_tab,
             T_symbol *actual_class, T_data_type dtype)
{{{

    // finding variable in local table
    T_symbol *sym = table_find(local_tab, iden, NULL);
    if (!sym || sym->symbol_type != is_var) {
        // variable not found in local table
        sym = table_find(symbol_tab, iden, actual_class);
    }

    // symbol is not variable or was not found
    if (!sym || sym->symbol_type != is_var) {
        terminate(DEFINITION_ERROR);
    }

    // checking data type
    // int to double accepted
    if (sym->attr.var->data_type == is_int && dtype == is_double)
        return 0;

    if (sym->attr.var->data_type != dtype) {
        terminate(TYPE_ERROR);
    }
    return 0;
}}}

T_symbol *add_constant(T_value value, struct T_Hash_symbol_table *symbol_tab,
                       T_data_type dtype)
{{{

    // temporary buffer
    char buf[128] = "";

    // creating a identifier name for constant
    if (dtype == is_int)
        sprintf(buf, "127%d", value.n);
    else if (dtype == is_int)
        sprintf(buf, "127%g", value.d);
    else
        sprintf(buf, "127%s", value.str);

    T_symbol *sym = table_find(symbol_tab, buf, NULL);
    // constant already in table, return pointer on it
    if (sym && sym->attr.var->data_type == dtype)
        return sym;

    // creating deep copy
    char *id = calloc(strlen(buf) + 1, 1);
    if (!id)
        terminate(INTERNAL_ERROR);
    strcpy(id, buf);
    sym = create_var(id, dtype);

    if (dtype == is_int || dtype == is_double)
        sym->attr.var->value = value;

    // string value is in identifier
    // setting constant flag
    sym->attr.var->is_const = true;
    table_insert(symbol_tab, sym);
    return sym;
}}}

// filling global symbol table with built-in class ifj
int fill_ifj16()
{{{
    char *ptr = malloc(6); // size of "ifj16"
    if (!ptr)
        terminate(INTERNAL_ERROR);
    strcpy(ptr, "ifj16");             // creates id ifj
    table_insert(symbol_tab, create_symbol(ptr, is_class)); // insert class ifj
    return 0;
}}}

// deletes whole local table
void local_table_remove(struct T_Hash_symbol_table **stab)
{{{
    if (*stab != NULL) {
        T_symbol* s;

        for (unsigned i = 0; i < (*stab)->size; i++) {
            while ((s = (*stab)->arr[i]) != NULL) {

                (*stab)->arr[i] = s->next;
                free((void*)s->id);

                if (s->attr.var->data_type == is_str &&
                    s->attr.var->value.str) {
                        free(s->attr.var->value.str);
                }
                free(s->attr.var);
                free(s);
            }
        }
        free(*stab);
        *stab = NULL;
    }
}}}

