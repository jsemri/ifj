/*
 * IFJ 2016
 * FIT VUT Brno
 * IFJ16 Interpret Project
 *
 * Authors:
 * Jakub   Semric     - xsemri00
 * Peter   Rusinak    - xrusin03
 * Krystof Rykala     - xrykal00
 * Martin  Mikan      - xmikan00
 * Martin  Polakovic  - xpolak33
 *
 * Unless otherwise stated, all code is licenced under a
 * GNU General Public License v2.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"
#include "debug.h"

// initialization of symbol table
T_symbol_table *table_init(unsigned size)
{{{
    T_symbol_table *ptr = calloc(sizeof(T_symbol_table) +
                                 size*sizeof(T_symbol), 1);
    if (ptr)
        ptr->size = size;
    return ptr;
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
    unsigned  h = 0;
    int c;
    while ((c = *key++))
        h = c + (h << 6) + (h << 16) - h;
    return h % size;
}}}


T_symbol *table_find_local(T_symbol_table *stab, char *key)
{{{
    unsigned index = hash(key, stab->size);
    T_symbol* item = stab->arr[index];

    while (item != NULL) {
        // member class and identifiers have to be same
        if (strcmp(item->id,key) == 0) {
            return item;
        }
        item = item->next;
    }
    return item;
}}}

T_symbol *table_find_simple(T_symbol_table *stab, char *key, T_symbol *mclass)
{{{
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
        char *str_class = key;
        // instance identifier
        char *str_inst = key + strlen(str_class) + 1;
        // if ifj16 found return class ifj16
        if (!strcmp(str_class, "ifj16")) {
            *dotptr = '.';
            return table_find_simple(stab, "ifj16", NULL);
        }
        // searching for class, id = mclass
        T_symbol *sym_class = table_find_simple(stab, str_class, NULL);
        // class not found
        if (!sym_class) {
            *dotptr = '.';
            return NULL;
        }
        // searching for variable in class `sym_class`
        T_symbol *item = table_find_simple(stab, str_inst, sym_class);
        *dotptr = '.';
        return item;
    }
    else {
        return table_find_simple(stab, key, mclass);
    }
}}}

// deletes whole table
void table_remove(T_symbol_table **stab)
{{{
    if (*stab != NULL) {
        T_symbol* s;

        for (unsigned i = 0; i < (*stab)->size; i++) {
            while ( (s = (*stab)->arr[i]) ) {

                (*stab)->arr[i] = s->next;
                if (s->symbol_type == is_var ) {
                    var_remove(s);
                }
                else if (s->symbol_type == is_func) {
                    func_remove(s);
                }
                else {
                    free(s->id);
                    free(s);
                }
            }
        }
        free(*stab);
        *stab = NULL;
    }
}}}


void heap_sort(char *str)
{{{
	int right = strlen(str)-1;

    for (int left = right/2; left >= 0; left--)
        sift_down(str, left, right);

    for (; right >= 1; right--) {
        str[0] ^= str[right];
        str[right] ^= str[0];
        str[0] ^= str[right];
        sift_down(str, 0, right - 1);
    }
}}}

void sift_down(char *str, int left, int right)
{{{
    int i = left;
    int j = 2 * i; //left son index
    char tmp = str[i];

    while (j <= right) {
        if (j < right && str[j] < str[j+1])
            j++;
        if (tmp >= str[j])
            break;
        else {
            str[i] = str[j];
            i = j;
            j *= 2;
        }
    }
    str[i] = tmp;
}}}

/**
 * Find a substring in a string
 * @param str The string where the substring will be searched
 * @param pattern The substring to be found
 * @return Index of the start of the first occurrence of pattern in the str
 *         or -1, if the pattern won't be found
 */
int find_kmp(char *str, char *substr)
{{{
    int sub_len = strlen(substr);
    int str_len = strlen(str);

    // Generate fail vector:
    int fail_vector[sub_len];

    fail_vector[0] = -1;
    for (int k = 1; k < sub_len; k++) {
        int r = fail_vector[k-1];
        while ((r > -1) && (substr[r] != substr[k-1]))
            r = fail_vector[r];
        fail_vector[k] = r + 1;
    }

    // Find the substring:
    int str_index = 0;
    int sub_index = 0;

    while ((str_index < str_len) && (sub_index < sub_len)) {
        if ((sub_index == -1) || (str[str_index] == substr[sub_index])) {
            str_index++;
            sub_index++;
        }
        else {
            sub_index = fail_vector[sub_index];
        }
    }

    return (sub_index == sub_len) ? str_index - sub_len: -1;
}}}

