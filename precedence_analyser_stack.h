/*
 * precedence_analyser_stack.h
 */

#ifndef IFJ_PRECEDENCE_ANALYSER_STACK_H
#define IFJ_PRECEDENCE_ANALYSER_STACK_H

#include "token.h"
#include "ial.h"
#include <stdbool.h>

typedef enum {
    PREC_EXP,       // E
    PREC_TOKEN,     // Token
    PREC_HANDLE,    // Start of handle (<)
    PREC_TOP        // Top of the stack ($)
} T_prec_stack_type;

typedef struct {
    T_prec_stack_type type;
    union {
        T_token *token;
        T_symbol *symbol;
    } ptr;
} T_prec_stack_entry;

typedef struct {
    unsigned last;
    unsigned capacity;
    T_prec_stack_entry *data;
} T_prec_stack;


/**
 * Inicializes an empty stack
 * @return A pointer to inicialized stack or NULL if the allocation fails
 */
T_prec_stack *prec_stack_new();

/**
 * Adds token to the top of the stack
 * @param s Stack
 * @param token Token
 * @return True, if the operation was succesful
 */
bool prec_stack_push_token(T_prec_stack *s, T_token *token);

/**
 * Adds expression to the top of the stack
 * @param s Stack
 * @param token Token that represents the expression
 * @return True, if the operation was succesful
 */
bool prec_stack_push_exp(T_prec_stack *s, T_symbol *symbol);

/**
 * Adds symbol to the top of the stack
 * @param s Stack
 * @param symbol Added symbol, one of PREC_EXP, PREC_STOP and PREC_TOP
 * @return True, if the operation was succesful
 */
bool prec_stack_push_symbol(T_prec_stack *s, T_prec_stack_type symbol);

/**
 * Return the type of the topmost token on the stack.
 * If no token is in the stack, TT_eof will be returned
 * @param s The stack
 * @return Type of the topmost token on the stack
 */
T_tokenType prec_stack_get_top_token_type(T_prec_stack *s);

/**
 * Removes all elements from the topmost handle to the top from the stack.
 * All elements except the handle will be moved to variable 'out'. If more than
 * 'max' elements should be moved there, the function will fail. The number of
 * filled elements will be strored in 'count'
 * @param s The stack
 * @param out Output array where the elements will be moved
 * @param max The capacity of array 'out'
 * @param count The number of actually written elements will be written here
 * @return True, if the operation was succesful
 */
bool prec_stack_reduce(T_prec_stack *s, T_prec_stack_entry out[], int max,
                       int *count);

/**
 * Adds handle before the first token on the stack.
 * @return True, if the operation was succesful
 */
bool prec_stack_add_handle(T_prec_stack *s);

/**
 * Checks if the stack is empty. The stack is considered empty if it contains
 * two elements: the end marker and one expression
 * @param s Stack
 * @return True, if the stack is empty
 */
bool prec_stack_is_empty(T_prec_stack *s);

/**
 * Prints the contest of the stack for debugging purposes
 * @param stack_ptr A pointer to the stack
 */
void prec_stack_print(T_prec_stack *stack_ptr);

/**
 * Deletes stack
 * @param stack_ptr A pointer to the stack that sould be deleted
 */
void prec_stack_free(T_prec_stack *stack_ptr);


#endif //IFJ_PRECEDENCE_ANALYSER_STACK_H
