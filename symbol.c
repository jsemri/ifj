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
#include "debug.h"
#include "ilist.h"

// global symbol table
T_symbol_table *symbol_tab;

T_symbol *create_func(char *id, T_data_type dtype)
{{{
    T_symbol *sym = create_symbol(id, is_func);
    T_func_symbol *func = calloc(1, sizeof(T_func_symbol));

    if (!func) {
        free(sym);
        terminate(INTERNAL_ERROR);
    }
    // setting return type
    func->data_type = dtype;

    // creating local table
    if (!(func->local_table = table_init(RANGE))) {
        free(func);
        free(sym);
        terminate(INTERNAL_ERROR);
    }

    // creating instruction list
    if (!(func->func_ilist = list_init())) {
        table_remove(&func->local_table);
        free(func);
        free(sym);
        terminate(INTERNAL_ERROR);
    }
    sym->attr.func = func;
    return sym;
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

void func_remove(T_symbol *sym)
{{{
    list_free(&sym->attr.func->func_ilist);
    local_table_remove(&sym->attr.func->local_table);
    if (sym->attr.func->arguments)
        free(sym->attr.func->arguments);
    free(sym->attr.func);
    free(sym->id);
    free(sym);
}}}

void var_remove(T_symbol *sym)
{{{
    if (sym->attr.var->data_type && sym->attr.var->value.str)
        free(sym->attr.var->value.str);
    free(sym->id);
    free(sym->attr.var);
    free(sym);
}}}

T_symbol *create_var(char *id, T_data_type dtype)
{{{
    T_symbol *sym = create_symbol(id, is_var);
    if (!(sym->attr.var = calloc(1, sizeof(T_var_symbol)))) {
        free(sym);
    }
    sym->attr.var->data_type = dtype;
    return sym;
}}}


T_symbol *create_var_uniq(T_data_type dtype)
{
    static int uniq_var_counter = 0;
    static int uniq_var_size = 2;
    static int uniq_var_capacity = 16;

    if (uniq_var_counter == uniq_var_capacity) {
        uniq_var_counter -= uniq_var_capacity;
        uniq_var_capacity *= 16;
        uniq_var_size++;
    }

    char *str = malloc((size_t) uniq_var_size);
    int rem = uniq_var_counter;
    for (int i = uniq_var_size - 2; i >= 0; i--) {
        str[i] = (char) ('a' + (rem & 15));
        rem = rem >> 4;
    }
    str[uniq_var_size] = 0;
    uniq_var_counter++;

    return create_var(str, dtype);
}

T_symbol *is_defined(char *iden, T_symbol_table *local_tab,
             T_symbol *actual_class, T_data_type dtype)
{{{

    // finding variable in local table
    T_symbol *sym = table_find_simple(local_tab, iden, NULL);

    if (!sym || sym->symbol_type != is_var) {
        // variable not found in local table
        sym = table_find(symbol_tab, iden, actual_class);
    }

    // symbol is not variable or was not found
    if (!sym || sym->symbol_type != is_var) {
        terminate(DEFINITION_ERROR);
    }

    // XXX variable cannot be void
    // this flag is used if we do not care about data type
    if (dtype == is_void)
        return sym;

    // checking data type
    // int to double accepted
    if (sym->attr.var->data_type == is_int && dtype == is_double)
        return sym;

    if (sym->attr.var->data_type != dtype) {
        terminate(TYPE_ERROR);
    }
    return sym;
}}}

T_symbol *add_constant(T_value *value, struct T_Hash_symbol_table *symbol_tab,
                       T_data_type dtype)
{{{

    static char w1, w2, w3;

    char buf[] = "&|a4ejg82|";
    w1 = rand() % 255 + 1;
    w2 = rand() % 149 + 1;
    w3 = rand() % 255 + 1;
    buf[0] = w1;buf[1] = w2; buf[2] = w3;
    T_symbol *sym = create_var(get_str(buf), dtype);


    sym->attr.var->value = *value;
    value->str = NULL;

    // string value is in identifier
    // setting constant flag
    sym->attr.var->is_const = true;
    sym->attr.var->initialized = true;
    table_insert(symbol_tab, sym);
    return sym;
}}}

T_symbol *symbol_copy(T_symbol *sym)
{{{
    T_symbol *new_symbol;
    char *id = get_str(sym->id);
     // only variable
    new_symbol = create_var(id, sym->attr.var->data_type);
    if (new_symbol->attr.var->data_type == is_str) {
        new_symbol->attr.var->value.str = get_str(sym->attr.var->value.str);
    }
    else {
        new_symbol->attr.var->value = sym->attr.var->value;
    }
    new_symbol->attr.var->initialized = sym->attr.var->initialized;
    return new_symbol;
}}}

// filling global symbol table with built-in class ifj
int fill_ifj16()
{{{
    char *ptr = get_str("ifj16");
    table_insert(symbol_tab, create_symbol(ptr, is_class)); // insert class ifj
    // insert register
    ptr = get_str("|accumulator|");
    T_symbol *s = create_var(ptr, is_int);
    table_insert(symbol_tab, s);
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
                    s->attr.var->value.str)
                {
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

char *get_str(const char *src)
{{{
    char *ptr = calloc(strlen(src) + 1, 1);
    if (!ptr)
        terminate(INTERNAL_ERROR);
    strcpy(ptr, src);
    return ptr;
}}}
