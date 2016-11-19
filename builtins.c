#include "symbol.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

void read_stdin(T_symbol *result) {
    char *buf;
    unsigned counter = 0;
    int c;
    char *endptr;

    if((buf = malloc(sizeof(char))) == NULL) {
        terminate(99);
    }

    while((c = fgetc(stdin)) != EOF || c != '\n') {
        // realloc if needed
        if(counter > 0) {
            if((buf = realloc(buf, sizeof(char) * (counter + 1))) == NULL) {
                terminate(99);
            }
        }
        // add char at end of buf
        buf[counter++] = c;
    }

    if (result->attr.var->data_type == is_int) {       
        result->attr.var->value.n = (int)strtol(buf, &endptr, 10);
        if (endptr != buf && *endptr != '\0') {
            free(buf);
            terminate(7);
        }
        free(buf);
    } else if (result->attr.var->data_type == is_double) {
        result->attr.var->value.d = strtod(buf, &endptr);
        if (endptr != buf && *endptr != '\0') {
            free(buf);
            terminate(7);
        }
        free(buf);
    } else if (result->attr.var->data_type == is_str) {
        result->attr.var->value.str = buf;
    }
}