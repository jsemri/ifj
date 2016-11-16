#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"

// initialization of symbol table
T_symbol_table *table_init(unsigned size)
{{{
    T_symbol_table* table = malloc(sizeof(T_symbol_table) +
                                   size * sizeof(T_symbol));
    if (table == NULL)
        return NULL;

    table->size = size;

    for (unsigned i = 0; i < size; i++)
    {
        table->arr[i] = NULL;
    }

    return table;
}}}


// inserts symbol item to symbol table
// returns pointer to the inserted symbol
T_symbol *table_insert(T_symbol_table *stab, T_symbol *s)
{{{
    unsigned index = hash(s->id, stab->size);
    s->next = stab->arr[index];
    stab->arr[index] = s;
    return s;
}}}


// hash- determines an index to hash table
unsigned hash(const char *key, unsigned size)
{{{
    unsigned int h=0;
    const unsigned char *p;
    for (p=(const unsigned char*)key; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % size;
}}}


// search for symbol with `key`
// returns a pointer to the searched symbol
// returns NULL if symbol was not found
T_symbol *table_find(T_symbol_table *stab, char *key, T_symbol *mclass)
{{{

    char *dotptr = strchr(key, '.');

    // only not NULL if full identifier, thus dot found
    if (dotptr) {
        // dividing key to two strings
        *dotptr = 0;
        // class identifier
        char *mclass = key;
        // instance identifier
        char *inst = key + strlen(mclass) + 1;
        // if ifj16 found return class ifj16
        if (!strcmp(mclass, "ifj16")) {
            *dotptr = '.';
            return table_find(stab, "ifj16", NULL);
        }
        T_symbol *item = table_find(stab, inst, table_find(stab, mclass, NULL));
        *dotptr = '.';
        return item;
    }
    else {
        unsigned index = hash(key, stab->size);
        T_symbol* item = stab->arr[index];

        while (item != NULL) {
            // member class and identifiers have to be same
            if (strcmp(item->id,key) == 0 && item->member_class == mclass) {
                return item;
            }
            item = item->next;
        }
        return item;
    }
}}}

// deletes whole table
void table_remove(T_symbol_table **stab)
{{{
    if (*stab != NULL) {
        T_symbol* s;

        for (unsigned i = 0; i < (*stab)->size; i++) {
            while ((s = (*stab)->arr[i]) != NULL) {

                (*stab)->arr[i] = s->next;
                if (s->id)
                    free((void*)s->id);

                if (s->symbol_type == is_var ) {
                    if (s->attr.var) {
                        if(s->attr.var->data_type == is_str &&
                                s->attr.var->value.str) {
                            free(s->attr.var->value.str);
                        }
                        free(s->attr.var);
                    }
                }
                else if (s->symbol_type == is_func && s->attr.func) {
                    local_table_remove(&s->attr.func->local_table);
                    if (s->attr.func->arguments)
                        free(s->attr.func->arguments);
                    free(s->attr.func);
                }
                free(s);
            }
        }
        free(*stab);
        *stab = NULL;
    }
}}}
