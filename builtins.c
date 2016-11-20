#include "symbol.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

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
        if ( *endptr && !count ) {
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
        if (!count && *endptr) {
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

}}}


