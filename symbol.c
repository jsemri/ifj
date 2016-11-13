#include "globals.h"
#include "symbol.h"
#include "ial.h"
#include "token.h"
#include "token_vector.h"
#include "scanner.h"
#include <string.h>
#include <stdlib.h>

#define BI_COUNT 9
char *arr_ifj16[] = {
    "readInt",  "readDouble", "readString", "print",
    "length", "substr", "compare", "find", "sort", "ifj16"
    };

// temporary pointer for build-in symbols
static T_symbol **sym_arr;
T_symbol_table *symbol_tab;

int find_var(const char *iden, T_symbol_table *local_tab,
             T_symbol *actual_class, T_data_types dtype)
{
    T_symbol *sym;
    // finding variable in local table
    sym = table_find(local_tab, iden, NULL);
    if (!sym || sym->symbol_type != is_var) {

        // variable not found in local table
        sym = table_find(symbol_tab, iden, actual_class);
        // variable not found in global table
        if (!sym || sym->symbol_type != is_var) {
            return DEFINITION_ERROR;
        }

    }

    // checking data type
    if (sym->data_type != dtype) {
        return TYPE_ERROR;
    }
    return 0;
}


// filling global symbol table with built-in class ifj
int fill_ifj16() {
    sym_arr = calloc(BI_COUNT+1, sizeof(T_symbol*));
    if (!sym_arr)
        return INTERNAL_ERROR;

    for (unsigned i = 0;i <= BI_COUNT;i++) {
        T_symbol *s = calloc(1, sizeof(T_symbol));
        if (!s) {
            for (unsigned j = 0; j < i; j++)
                free(sym_arr[j]);
            free(sym_arr);
        }
        s->id = arr_ifj16[i];
        sym_arr[i] = s;
    }
    table_insert(symbol_tab, sym_arr[BI_COUNT]);
    for (unsigned i = 0; i < BI_COUNT;i++) {
        sym_arr[i]->member_class = sym_arr[BI_COUNT];
        sym_arr[i]->symbol_type = is_func;
        table_insert(symbol_tab, sym_arr[i]);
    }

    return 0;
}

// just sets pointers to null
void remove_ifj16() {
    for (unsigned i = 0; i <= BI_COUNT;i++) {
        sym_arr[i]->id = NULL;
    }
    free(sym_arr);
}
// deletes whole local table
void local_table_remove(struct T_Hash_symbol_table **stab) {
    if (*stab != NULL) {
        T_symbol* s;

        for (unsigned i = 0; i < (*stab)->size; i++) {
            while ((s = (*stab)->arr[i]) != NULL) {

                (*stab)->arr[i] = s->next;
                free((void*)s->id);

                if (s->data_type == is_str && s->attr.var->value.str) {
                        str_free(s->attr.var->value.str);
                }
                free(s->attr.var);
                free(s);
            }
        }
        free(*stab);
        *stab = NULL;
    }
}
