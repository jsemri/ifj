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
 * @file stack.h
 * Stack and frame structures and functions.
 *
 */

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "symbol.h"
#include "ial.h"

/// Stack
typedef struct {
    void **data;  // stack array
    int used;     // stack pointer
    int size;     // maximal size
} T_stack;

/// frame
typedef struct {
    T_symbol_table *local_tab;    // local table
    T_data_type dtype;            // data type
} T_frame;

/**
 * Removes a frame.
 *
 * @param frame  frame to be removed
 */
void remove_frame(T_frame **frame);

/**
 * Removes frame at top of the stack.
 *
 */
void remove_frame_from_stack();

/**
 * Creates a frame.
 *
 * @param func source function
 */
void create_frame(T_symbol *func);

/**
 * Initialization of stack.
 *
 * @return pointer to new stack
 */
T_stack *stack_init();

/**
 * Pushes item to stack.
 *
 * @param stack place where to push
 * @param item data what to push
 */
void stack_push(T_stack *stack, void *item);

/**
 * Show a value at top of stack.
 *
 * @param stack simple stack
 * @return item at the top of stack
 */
void *stack_top(T_stack *stack);

/**
 * Erases a value at top of stack.
 *
 * @param stack simple stack
 */
void stack_pop(T_stack *stack);

/**
 * Erases a  stack.
 *
 * @param stack simple stack
 * @param is_frame_stack determines if it is frame stack
 */
void stack_remove(T_stack **stack, bool is_frame_stack);

/**
 * Copies a value from symbol.
 *
 * @param dst destination symbol
 * @param src source symbol
 */
void copy_value(T_symbol *dst, T_symbol *src);

/**
 * Checks if stack is empty.
 *
 * @param stack simple stack
 * @return return true if stack is empty false otherwise
 */
bool is_empty(T_stack *stack);

#endif

