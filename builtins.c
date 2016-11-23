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

#include "symbol.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include "ial.h"

#define BUF_SIZE 32
#define is_real(s) (s->attr.var->data_type == is_double)
#define is_init(s) (s->attr.var->initialized)


void clear_buffer(T_symbol *sym)
{{{
    if (sym->attr.var->value.str)
        free(sym->attr.var->value.str);
    sym->attr.var->value.str = NULL;
}}}

void read_stdin(T_symbol *result, T_data_type dtype)
{{{
    char *buf;
    unsigned size = BUF_SIZE;
    unsigned count = 0;
    int c;
    char *endptr;

    if((buf = calloc(count, 1)) == NULL) {
        terminate(99);
    }

    while((c = fgetc(stdin)) != EOF && c != '\n') {
        // realloc if needed
        if(count >= size) {
            void *ptr;
            if((ptr = realloc(buf, size*2)) == NULL) {
                free(buf);
                terminate(99);
            }
            buf = ptr;
            size *= 2;
        }
        // add char at end of buf
        buf[count++] = c;
    }

    buf[count] = 0;

    if (dtype == is_int) {
        int n = (int)strtol(buf, &endptr, 10);
        if ( *endptr || !count ) {
            free(buf);
            terminate(7);
        }
        free(buf);
        if (result) {
            // double or int
            if (is_real(result))
                result->attr.var->value.d = n;
            else
                result->attr.var->value.n = n;
        }

    }
    else if (dtype == is_double) {
        double d = strtod(buf, &endptr);
        if (!count || *endptr) {
            free(buf);
            terminate(7);
        }
        free(buf);
        if (result) {
            // double or int
            if (is_real(result))
                result->attr.var->value.d = d;
            else
                result->attr.var->value.n = d;
        }
    }
    else {
        // clearing buffer
        clear_buffer(result);
        result->attr.var->value.str = buf;
    }
    result->attr.var->initialized = true;
}}}

void substr(T_symbol *sym1, T_symbol *sym2, T_symbol *sym3, T_symbol *result)
{{{

    // TODO overlapping

    // checking initialization flag
    if (!is_init(sym1) || !is_init(sym2) || !is_init(sym3))
    {
        terminate(8);
    }

    int n, i;
    char *buf = sym1->attr.var->value.str;

    if (is_real(sym2))
        i = sym2->attr.var->value.d;
    else
        i = sym2->attr.var->value.n;

    if (is_real(sym3))
        n = sym3->attr.var->value.d;
    else
        n = sym3->attr.var->value.n;

    // overreach or negative numbers
    if (n + i > (int)strlen(buf) || n < 0 || i < 0) {
        terminate(10);
    }

    char *res;
    if(!(res = calloc(n + 1, 1))) {
        terminate(99);
    }

    for (int j = 0;j < n;j++)
        res[j] = buf[i+j];

    clear_buffer(result);
    result->attr.var->value.str = res;
    result->attr.var->initialized = true;
}}}


void compare(T_symbol *sym1, T_symbol *sym2, T_symbol *result)
{{{
    if (!is_init(sym1) || !is_init(sym2))
    {
        terminate(8);
    }

    int cmp = strcmp(sym1->attr.var->value.str, sym2->attr.var->value.str);
    cmp = cmp > 0 ? 1 : cmp;
    cmp = cmp < 0 ? -1 : cmp;

    if (is_real(result))
        result->attr.var->value.d = cmp;
    else
        result->attr.var->value.n = cmp;

    result->attr.var->initialized = true;
}}}


void length(T_symbol *sym1, T_symbol *result)
{{{

    if (!is_init(sym1))
    {
        terminate(8);
    }

    int len = strlen(sym1->attr.var->value.str);

    if (is_real(result))
        result->attr.var->value.d = len;
    else
        result->attr.var->value.n = len;

    result->attr.var->initialized = true;
}}}

void concat(T_symbol *sym1, T_symbol *sym2, T_symbol *result)
{{{

    if (!is_init(sym1) || !is_init(sym2))
    {
        terminate(8);
    }

    // buffers are static because number can't reach over 31 characters
    char *s1, *s2, buf1[BUF_SIZE], buf2[BUF_SIZE];

    if (sym1->attr.var->data_type == is_str) {
        s1 = sym1->attr.var->value.str;
    }
    else {
        if (is_real(sym1))
            snprintf(buf1, BUF_SIZE-1, "%g", sym1->attr.var->value.d);
        else
            snprintf(buf1, BUF_SIZE-1, "%d", sym1->attr.var->value.n);
        s1 = buf1;
    }

    if (sym2->attr.var->data_type == is_str) {
        s2 = sym2->attr.var->value.str;
    }
    else {
        if (is_real(sym2))
            snprintf(buf2, BUF_SIZE-1, "%g", sym2->attr.var->value.d);
        else
            snprintf(buf2, BUF_SIZE-1, "%d", sym2->attr.var->value.n);
        s2 = buf2;
    }

    char *str = calloc(strlen(s1) + strlen(s2) + 1,1);

    if (!str)
        terminate(99);

    strcpy(str, s1);
    strcpy(str + strlen(str), s2);

    clear_buffer(result);
    result->attr.var->value.str = str;
    result->attr.var->initialized = true;
}}}

void sort(T_symbol *sym, T_symbol *result)
{{{

    if (!is_init(sym))
    {
        terminate(8);
    }

    char *str = get_str(sym->attr.var->value.str);
    heap_sort(str);
    clear_buffer(result);
    result->attr.var->value.str = str;
    result->attr.var->initialized = true;
}}}

void find(T_symbol *sym1, T_symbol *sym2, T_symbol *result)
{{{

    if (!is_init(sym1) || !is_init(sym2))
    {
        terminate(8);
    }

    char *s1 = sym1->attr.var->value.str;
    char *s2 = sym2->attr.var->value.str;

    if (is_real(result)) {
        result->attr.var->value.d = find_kmp(s1, s2);
    }
    else {
        result->attr.var->value.n = find_kmp(s1, s2);
    }

    result->attr.var->initialized = true;
}}}

