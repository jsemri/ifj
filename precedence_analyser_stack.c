/*
 * precedence_analyser_stack.c
 */

#include "precedence_analyser_stack.h"
#include "globals.h"
#include <stdlib.h>

#define INIT_CAPACITY 20

static bool prec_stack_resize_if_needed(T_prec_stack *s);

/**
 * Inicializes an empty stack
 * @return A pointer to inicialized stack or NULL if the allocation fails
 */
T_prec_stack *prec_stack_new() {
    T_prec_stack *t = malloc(sizeof(T_prec_stack));
    if (t == NULL)
        return NULL;
    t->last = 0;
    t->capacity = INIT_CAPACITY;
    t->data = malloc(INIT_CAPACITY * sizeof(T_prec_stack_entry));
    if (t->data == NULL) {
        free(t);
        return NULL;
    }
    return t;
}

/**
 * Resizes the stack to fit one more item. If the stack is already big enough,
 * does nothing
 * @param s A pointer to the resized stack
 * @return True, if the operation was succesful
 */
static bool prec_stack_resize_if_needed(T_prec_stack *s) {
    if (s->last < s->capacity)
        // No need to resize the stack
        return true;
    T_prec_stack_entry *data = realloc(s->data, 2 * s->capacity *
                                                sizeof(T_prec_stack_entry));
    if (data == NULL)
        // Resizing failed
        return false;

    s->capacity *= 2;
    s->data = data;
    return true;
}

/**
 * Adds token to the top of the stack
 * @param s Stack
 * @param token Token
 * @return True, if the operation was succesful
 */
bool prec_stack_push_token(T_prec_stack *s, T_token *token) {
    if (!prec_stack_resize_if_needed(s))
        return false;
    T_prec_stack_entry new_entry;
    new_entry.type = PREC_TOKEN;
    new_entry.ptr.token = token;
    s->data[s->last++] = new_entry;
    return true;
}

/**
 * Adds expression to the top of the stack
 * @param s Stack
 * @param token Token that represents the expression
 * @return True, if the operation was succesful
 */
bool prec_stack_push_exp(T_prec_stack *s, T_symbol *symbol) {
    if (!prec_stack_resize_if_needed(s))
        return false;
    T_prec_stack_entry new_entry;
    new_entry.type = PREC_EXP;
    new_entry.ptr.symbol = symbol;
    s->data[s->last++] = new_entry;
    return true;
}

/**
 * Adds symbol to the top of the stack
 * @param s Stack
 * @param symbol Added symbol, either PREC_STOP or PREC_TOP
 * @return True, if the operation was succesful
 */
bool prec_stack_push_symbol(T_prec_stack *s, T_prec_stack_type symbol) {
    if (!prec_stack_resize_if_needed(s))
        return false;
    T_prec_stack_entry new_entry;
    new_entry.type = symbol;
    new_entry.ptr.token = NULL;
    s->data[s->last++] = new_entry;
    return true;
}

/**
 * Return the type of the topmost token on the stack.
 * If no token is in the stack, TT_eof will be returned
 * @param s The stack
 * @return Type of the topmost token on the stack
 */
T_tokenType prec_stack_get_top_token_type(T_prec_stack *s) {
    for (int i = s->last - 1; i >= 0; i--) {
        if (s->data[i].type == PREC_TOKEN)
            return s->data[i].ptr.token->type;
        if (s->data[i].type == PREC_TOP)
            return TT_eof;
    }
    return TT_eof;
}

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
                       int *count) {
    int j = 0;
    for (int i = s->last - 1; i >= 0; i--) {
        if (s->data[i].type == PREC_HANDLE) {
            s->last = (unsigned) i;
            *count = j;
            return true;
        }
        if (j == max)
            return false;
        out[j++] = s->data[i];
    }
    return false;
}

/**
 * Adds handle before the first token on the stack.
 * @return True, if the operation was succesful
 */
bool prec_stack_add_handle(T_prec_stack *s) {
    if (!prec_stack_resize_if_needed(s))
        return false;
    bool result = false;
    for (int i = s->last - 1; i >= 0; i--) {
        if (s->data[i].type == PREC_TOKEN || s->data[i].type == PREC_TOP) {
            T_prec_stack_entry e;
            e.type = PREC_HANDLE;
            e.ptr.token = NULL;
            s->data[i+1] = e;
            result = true;
            break;
        }
        s->data[i+1] = s->data[i];
    }
    s->last++;
    return result;
}

/**
 * Checks if the stack is empty. The stack is considered empty if it contains
 * two elements: the end marker and one expression
 * @param s Stack
 * @return True, if the stack is empty
 */
bool prec_stack_is_empty(T_prec_stack *s) {
    return s->last == 2 && s->data[0].type == PREC_TOP
           && s->data[1].type == PREC_EXP;
}

/**
 * Prints the contest of the stack for debugging purposes
 * @param stack_ptr A pointer to the stack
 */
void prec_stack_print(T_prec_stack *stack_ptr) {
    printf("Obsah zásobníku\n************************\n");
    for (int i = stack_ptr->last - 1; i >= 0; i--) {
        T_prec_stack_type type = stack_ptr->data[i].type;
        if (type == PREC_TOKEN) {
            T_token *t = stack_ptr->data[i].ptr.token;
            switch (t->type) {
                case TT_int:
                    printf("(%2d) Token (int=%d)\n", i, t->attr.n);
                    break;
                case TT_double:
                    printf("(%2d) Token (double=%g)\n", i, t->attr.d);
                    break;
                case TT_plus:
                    printf("(%2d) Token (+)\n", i);
                    break;
                case TT_mul:
                    printf("(%2d) Token (*)\n", i);
                    break;
                default:
                    printf("(%2d) Token (jiný)\n", i);
            }
        }
        else if (type == PREC_EXP) {
            T_symbol *s = stack_ptr->data[i].ptr.symbol;
            switch (s->data_type) {
                case is_int:
                    printf("(%2d) Expression (int=%d)\n", i,
                           s->attr.var->value.num);
                    break;
                case is_double:
                    printf("(%2d) Expression (double=%g)\n", i,
                           s->attr.var->value.d);
                    break;
                default:
                    printf("(%2d) Expression (jiný)\n", i);
            }
        }
        else if (type == PREC_HANDLE)
            printf("(%2d) <\n", i);
        else if (type == PREC_TOP)
            printf("(%2d) $\n", i);
    }
}

/**
 * Deletes stack
 * @param stack_ptr A pointer to the stack that sould be deleted
 */
void prec_stack_free(T_prec_stack *stack_ptr) {
    free(stack_ptr->data);
    free(stack_ptr);
}
