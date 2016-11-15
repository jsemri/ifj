#include <stdlib.h>
#include "token.h"

T_token* token_new() {
    return calloc(1,sizeof(T_token));
}

void token_clear(T_token *t) {
    if (t) {
        // check if token is identifier or string
        if (t->type == TT_id || t->type == TT_string) {
            free(t->attr.str);
            t->attr.str = NULL;
        }
        t->type = TT_empty;
    }
}

void token_free(T_token **t) {
    if (*t) {
        // check if token is identifier or string
        if ((*t)->type == TT_id || (*t)->type == TT_string)
            free((*t)->attr.str);
        free(*t);
        *t = NULL;
    }
}

