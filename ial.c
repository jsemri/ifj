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
	int n = strlen(str) - 1;
	int left = n / 2;
	int right = n;
	char tmp;
    //establishment of heap
    for (int i = left; i >= 0; i--) {
        sift_down(str, i, right);
    }
    //heap-sort
    for (right = n; right >= 1; right--) {
        tmp = str[0];
        str[0] = str[right];
        str[right] = tmp;
        sift_down(str, 0, right - 1);
    }
}}}

void sift_down(char *str, int left, int right)
{{{
    int i = left;
    int j = 2 * i; //left son index
    char tmp = str[i];
    bool cont = j <= right;

    while (cont) {
        if (j < right)
            if (str[j] < str[j + 1]) {
                j = j + 1;
            } // if
        if (tmp >= str[j])
            cont = false;
        else {
            str[i] = str[j];
            i = j;
            j = 2 * i;
            cont = j <= right;
        }
    }
    str[i] = tmp;
}}}

int find_kmp(char *str, char *pattern)
{{{
    unsigned k;
    int r;
    int fail_vector[strlen(pattern)];

    fail_vector[0] = -1;
    for (k = 1; k < strlen(pattern); k++) {
        r = fail_vector[k-1];
        while ((r > -1) && (pattern[r] != pattern[k-1]))
            r = fail_vector[r];
        fail_vector[k] = r + 1;
    }

    unsigned string_index = 0;
    unsigned pattern_index = 0;

    while ((string_index < strlen(str)) && (pattern_index < strlen(pattern))) {
        if ((pattern_index == 0) || (str[string_index] == pattern[pattern_index])) {
            string_index++;
            pattern_index++;
        }
        else
            pattern_index = fail_vector[pattern_index];
    }
    if (pattern_index == strlen(pattern))
        return string_index - strlen(pattern);
    else
        return -1;
}}}

