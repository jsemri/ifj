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

/**
 * @file token_vector.h
 * @brief File contains structure and functions of token vector.
 */

#ifndef TOKEN_VECTOR
#define TOKEN_VECTOR

#include "token.h"

/// Structure token vector, contains array of tokens.
typedef struct T_token_vector {
    unsigned size;      // allocated space
    unsigned last;      // first free index
    T_token *arr;       // array of tokens
} *token_vector;        // pointer to vector

/**
 * @brief Initializes token vector.
 *
 * @return Pointer to token vector, @c null when allocation fails.
 */
token_vector token_vec_init();

/**
 * @brief Inserts token to a token vector.
 *
 * @param tvect vector of tokens
 * @param token inserted token
 * @return @c 0 on success, @c 1 on failure
 */
int token_push_back(token_vector tvect, const T_token *token);

/**
 * @brief Deletes token vector.
 *
 * @param tvect vector of tokens
 *
 */
void token_vec_delete(token_vector tvect);

#endif
