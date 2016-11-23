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

#include <stdlib.h>
#include "token.h"

T_token* token_new() {
    return calloc(1,sizeof(T_token));
}

void token_clear(T_token *t) {
    if (t) {
        // check if token is identifier or string
        if (t->type == TT_id || t->type == TT_string ||
            t->type == TT_fullid) {
            free(t->attr.str);
            t->attr.str = NULL;
        }
        t->type = TT_empty;
    }
}

void token_free(T_token **t) {
    if (*t) {
        // check if token is identifier or string
        if ((*t)->type == TT_id || (*t)->type == TT_string ||
             (*t)->type == TT_fullid)
            free((*t)->attr.str);
        free(*t);
        *t = NULL;
    }
}

