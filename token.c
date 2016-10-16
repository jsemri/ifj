#include <stdlib.h>
#include "string.h"
#include "token.h"

T_token* token_new() {
    return calloc(1,sizeof(T_token));
}

void token_clear(T_token *t) {
    if (t)
        if (t->type == TT_id || t->type == TT_string)
            str_free(&t->str);
}

void token_free(T_token **t) {
    if (*t) {
         if ((*t)->type == TT_id || (*t)->type == TT_string)
            str_free(&((*t)->str));
        free(*t);
        *t = NULL;
    }
}
