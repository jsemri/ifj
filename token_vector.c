#include "token_vector.h"
#include <stdlib.h>
#include "globals.h"

#define INITIAL_VECTOR_SIZE 16

token_vector token_vec_init() {
    // initialization of vector
    token_vector ptr = calloc(1, sizeof(struct T_token_vector));
    if (!ptr)
        terminate(INTERNAL_ERROR);

    ptr->arr = calloc(INITIAL_VECTOR_SIZE, sizeof(T_token));
    // allocating token buffer
    if (!ptr->arr) {
        free(ptr);
        terminate(INTERNAL_ERROR);
    }
    // setting initial parameters
    ptr->size = INITIAL_VECTOR_SIZE;
    ptr->last = 0;
    return ptr;
}

void token_vec_delete(token_vector tvect) {
    // deleting all strings
    for (unsigned i = 0;i < tvect->last;i++) {
        T_token *t = &tvect->arr[i];
        if (t->type == TT_id || t->type == TT_string)
            str_free(t->attr.str);
    }
    // deleting token array
    free(tvect->arr);
    // deleting vector
    free(tvect);
}

int token_push_back(token_vector tvect, const T_token *token) {

    // reallocation if needed
    if (tvect->size == tvect->last) {
        void *p = realloc(tvect->arr, 2*tvect->size*sizeof(T_token));
        if (!p) {
            return 1;
        }
        // changing allocated space
        tvect->arr = p;
        tvect->size *= 2;
    }

    // temporary pointer
    T_token *p = &tvect->arr[tvect->last];
    if (token->type == TT_id ||token->type == TT_string) {
        // checking string initialization and string copying
        if (!(p->attr.str = str_init())) {
            token_vec_delete(tvect);
            terminate(INTERNAL_ERROR);
        }

        if (-1 == str_copy(p->attr.str,token->attr.str)) {
            str_free(p->attr.str);
            token_vec_delete(tvect);
            terminate(INTERNAL_ERROR);
        }

    }
    else {
        p->attr =token->attr;
    }
    p->type =token->type;

    tvect->last++;
    return 0;
}
