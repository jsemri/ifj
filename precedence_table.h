/**
 * precedence_table.h
 *
 * This file contains definitions of the precedence table.
 *
 * Use function get_precedence() to get an entry from this table
 */

#ifndef PRECEDENCE_TABLE_H
#define PRECEDENCE_TABLE_H

#include "token.h"

/**
 * The count of token types supported by this table (this number is used
 * a size of the table).
 */
#define ALLOWED_TOKENS_SIZE 14


/**
 * Data type of each cell of the precedence table.
 */
typedef enum {
    PREC_LOWER,
    PREC_GREATER,
    PREC_EQUAL,
    PREC_INVALID
} T_precedence_state;


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
                                  T_tokenType input_token);

#endif