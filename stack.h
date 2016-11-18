/**
 *
 * @file stack.h
 *
 */
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "symbol.h"
#include "ial.h" 

typedef struct {
    void **data;
    unsigned used;  // stack pointer
    unsigned size;  // maximal size
} T_stack;

typedef struct {
    T_symbol_table *local_tab;    // local table
    T_value ret_val;              // return value
    T_data_type dtype;            // data type
} T_frame;

void remove_frame(T_frame **frame);

void remove_frame_from_stack(T_stack *stack);

void create_frame(T_symbol *func, T_stack *stack);

T_stack *stack_init();

void stack_push(T_stack *stack, void *item);

void *stack_top(T_stack *stack);

void stack_pop(T_stack *stack);

void stack_remove(T_stack **stack, bool is_frame_stack);

#endif

