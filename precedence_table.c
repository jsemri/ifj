/**
 * precedence_table.c
 *
 * This file contains definitions of the precedence table.
 */

#include "precedence_table.h"
static int get_table_index(T_tokenType token);

/*
 * List of supported types of tokens. The values of this list are used
 * as "labels" in the precedence table. The index of each value in this list
 * represents the actual position of the item in precedence_table.
 *
 * The list must have exactly ALLOWED_TOKENS_SIZE items.
 */
T_tokenType allowed_tokens[ALLOWED_TOKENS_SIZE] = {
    TT_plus,
    TT_minus,
    TT_div,
    TT_mul,
    TT_equal,
    TT_notEq,
    TT_lesser,
    TT_greater,
    TT_lessEq,
    TT_greatEq,
    TT_lBracket,
    TT_rBracket,
    TT_int,
    TT_eof
};

/*
 * Contains the data of the precedence table.
 *
 * See the project's documentation or
 * https://github.com/atepr/ifj/wiki/Preceden%C4%8Dn%C3%AD-tabulka
 * for a visual representation of this table.
 */
T_precedence_state precedence_table[] = {
    PREC_GREATER, PREC_GREATER, PREC_LOWER, PREC_LOWER, PREC_GREATER,
    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_GREATER, PREC_GREATER, PREC_LOWER, PREC_LOWER, PREC_GREATER,
    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_INVALID,
    PREC_INVALID, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_INVALID,
    PREC_INVALID, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_GREATER,
    PREC_GREATER, PREC_INVALID, PREC_INVALID, PREC_INVALID, PREC_INVALID,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_GREATER,
    PREC_GREATER, PREC_INVALID, PREC_INVALID, PREC_INVALID, PREC_INVALID,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_GREATER,
    PREC_GREATER, PREC_INVALID, PREC_INVALID, PREC_INVALID, PREC_INVALID,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_GREATER,
    PREC_GREATER, PREC_INVALID, PREC_INVALID, PREC_INVALID, PREC_INVALID,
    PREC_LOWER, PREC_GREATER, PREC_LOWER, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER,
    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER,
    PREC_LOWER, PREC_EQUAL, PREC_LOWER, PREC_INVALID,

    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_INVALID, PREC_GREATER, PREC_INVALID, PREC_GREATER,

    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER,
    PREC_INVALID, PREC_GREATER, PREC_INVALID, PREC_GREATER,

    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER,
    PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER, PREC_LOWER,
    PREC_LOWER, PREC_INVALID, PREC_LOWER, PREC_INVALID,
};


/**
 * Returns the precedence state for tokens stack_token and input_token.
 *
 * In other words, this function will return the cell in the stack_token-th row
 * and the input_token-th column of the precedence table.
 *
 * @param stack_token Type of token on the top of the stack
 * @param input_token Type of the next token on input
 * @return The precedence state, one of: PREC_LOWER, PREC_GREATER, PREC_EQUAL
 *                                       and PREC_INVALID
 */
T_precedence_state get_precedence(T_tokenType stack_token,
                                  T_tokenType input_token) {
    int index1 = get_table_index(stack_token);
    int index2 = get_table_index(input_token);
    if (index1 < 0 || index2 < 0) {
        return PREC_INVALID;
    }
    return precedence_table[ALLOWED_TOKENS_SIZE * index1 + index2];
}

/**
 * Translates the type of token to an index of the item in precedence table.
 * @param token Token to be traslated
 * @return Index of the token
 */
static int get_table_index(T_tokenType token) {
    for (int i = 0; i < ALLOWED_TOKENS_SIZE; i++) {
        if (allowed_tokens[i] == token) {
            return i;
        }
    }
    return -1;
}