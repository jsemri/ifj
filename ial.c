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
T_symbol *table_find(T_symbol_table *stab, const char *key, T_symbol *mclass) {
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

// deletes whole table
void table_remove(T_symbol_table **stab) {
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
                            str_free(s->attr.var->value.str);
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
}

/**
 * \brief Functions sorts strings.
 *
 * @param   src    Pointer to string.
 * @param   dest   Pointer to string, where sorted string will be stored.
 *
 * @return  HEAPSORT_OK     If success.
 * @return  HEAPSORT_FAIL   If duplication of new new string failed.
 */
int heap_sort(T_string *src, T_string *dest);
{

	//FAIL of HEAPSORT - doplnit

	int n = dest->length - 1;
	int left = n / 2;
	int right = n;
	char tmp;
    //establishment of heap   
    for (int i = left; i >= 0; i--) {
        sift_down(dest, i, right);
    }
    //heap-sort
    for (right = n; right >= 1; right--) {
        tmp = dest->string[0];
        dest->string[0] = dest->string[right];
        dest->string[right] = tmp;
        sift_down(dest, 0, right - 1);
    }

    return HEAPSORT_OP_OK;
}

/**
 * \brief Function for heapsort, reeastablishment of heap.
 * @param   string   Pointer to string.
 * @param   left     Index of the most left node.
 * @param   right    Index of the most righ node.
 */
void sift_down(T_string *string, int left, int right)
{   
    int i = left;
    int j = 2 * i; //left son index
    char tmp = string->string[i];
    bool cont = j <= right;
    
    while (cont) {
        if (j < right)
            if (string->string[j] < string->string[j + 1]) {
                j = j + 1;
            } // if
        if (tmp >= string->string[j])
            cont = false;
        else {
            string->string[i] = string->string[j];
            i = j;
            j = 2 * i;
            cont = j <= right;
        }
    }
    string->string[i] = tmp;
}

/**
 * \brief Function finds substring in string using Knuth-Morris-Pratt algorithm
 *		  and returns its position.
 *        If substring is not found, function returns -1.
 *
 * @param   string      Pointer to string.
 * @param   pattern   Pointer to substring.
 *
 * @return  Index of first occurrence substring in string.
 */
int find_kmp(T_string *string, T_string *pattern)