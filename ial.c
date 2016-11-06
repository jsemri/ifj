#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string.h"
#include "ial.h"

// initialization of symbol table
T_symbol_table *table_init(unsigned size) {
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
}


// inserts symbol item to symbol table
// returns pointer to the inserted symbol
T_symbol *table_insert(T_symbol_table *stab, T_symbol *s) {
    unsigned index = hash(s->id, stab->size);
    s->next = stab->arr[index];
    stab->arr[index] = s;
    return s;
}


// hash- determines an index to hash table
unsigned hash(const char *key, unsigned size) {
    unsigned int h=0;
    const unsigned char *p;
    for (p=(const unsigned char*)key; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % size;
}


// search for symbol with `key`
// returns a pointer to the searched symbol
// returns NULL if symbol was not found
T_symbol *table_find(T_symbol_table *stab, const char *key) {
    unsigned index = hash(key, stab->size);
    T_symbol* item = stab->arr[index];

    while (item != NULL) {
        if (strcmp(item->id,key) == 0) {
            return item;
        }
        item = item->next;
    }

    return item;
}

// deletes whole table
void table_remove(T_symbol_table *stab) {
    if (stab != NULL) {
        T_symbol* s;

        for (unsigned i = 0; i < stab->size; i++) {
            while ((s = stab->arr[i]) != NULL) {
                stab->arr[i] = s->next;
                free((void*)s->id);
                if (s->symbol_type == isvar && s->data_type == isstr)
                    str_free(s->value.str);
                free(s);
            }
        }
        free(stab);
    }
}

