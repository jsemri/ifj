#ifndef TOKEN_VECTOR
#define TOKEN_VECTOR

#include "token.h"

// structure token vector, contains array of T_token
typedef struct T_token_vector {
    unsigned size;      // allocated space
    unsigned last;      // first free index
    T_token *arr;       // array of tokens
} *token_vector;        // pointer to vector

// initializes token vector
token_vector token_vec_init();
// pushes back token to a token vector
int token_push_back(token_vector tvect, const T_token *t);
// deletes token vector
void token_vec_delete(token_vector tvect);

#endif
