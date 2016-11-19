#include "symbol.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 32

void read_stdin(T_symbol *result, T_data_type dtype) {
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
            if (result->attr.var->data_type == is_int)
                result->attr.var->value.n = n;
            else
                result->attr.var->value.d = n;
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
            if (result->attr.var->data_type == is_int)
                result->attr.var->value.n = d;
            else
                result->attr.var->value.d = d;
        }
    }
    else {
        result->attr.var->value.str = buf;
    }
    result->attr.var->initialized = true;
}
