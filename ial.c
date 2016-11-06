#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"

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

    T_symbol* item_pom = table_find(stab, s->id);






    if(item_pom == NULL){
        T_symbol* item;

        item = malloc(sizeof(T_symbol));
        if (item == NULL)
        {
            return NULL;
        }

        item->id = s->id;
        item->symbol_type = s->symbol_type;
        item->data_type = s->data_type;
        item->is_def = s->is_def;
        item->value = s->value;
        item->symbol_attr = s->symbol_attr;
        item->member_class = s->member_class;
        item->next = stab->arr[hash(s->id,stab->size)];
        //arr[hash(symbol->id)]; // FIXME ??

        return item;
    } else {
        return NULL;
    }


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
    T_symbol* item_ptr = NULL;

    if (item == NULL) {
        return NULL;
    }
    else {
        while (item != NULL) {
            if (strcmp(item->id,key) == 0) {
                item_ptr = item;
                break;
            }
            else {
                item_ptr = item;
                item = item->next;
            }
        }

        if(item != item_ptr)
        {
            return NULL;
        }

    }
    return item;
}

// deletes whole table
void table_remove(T_symbol_table *stab) {
    if (stab != NULL) {
        T_symbol* s = NULL;

        for (unsigned i = 0; i < stab->size; i++) {
            while ((s = stab->arr[i]) != NULL) {
                stab->arr[i] = s->next;
                free(s->id);
                if (s->data_type == isstr)
                    str_free(s->value.str);
                free(s);
            }
        }

        free(stab);
    }
}

