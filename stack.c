

#include "globals.h"
#include "stack.h"
#include <stdbool.h>
#include <stdlib.h>

#define INIT_SIZE   128
#define GROWTH      8

T_stack *stack_init() {
    T_stack *stack = calloc(1, sizeof(T_stack));
    stack->data = calloc(INIT_SIZE, sizeof(void*));
    stack->used = 0;
    stack->size = INIT_SIZE - 1;
    return stack;
}

void stack_push(T_stack *stack, void *item) {
    if (stack->used+1 == stack->size) {
        void *p = realloc(stack->data, stack->size + GROWTH);
        stack->data = p;
        stack->size+=GROWTH;
    }
    stack->data[++stack->used] = item;
}

void *stack_top(T_stack *stack) {
    return stack->data[stack->used];
}

void stack_pop(T_stack *stack) {
    if (stack->used)
        stack->data[stack->used--] = NULL;
}

bool is_empty(T_stack *stack) {
    return !stack->used;
}

void stack_remove(T_stack **stack, bool is_frame_stack) {
    if (*stack) {
        if (is_frame_stack) {
            while (!is_empty(*stack)) {
                // remove frame
                T_func_symbol *frame = stack_top(*stack);
                local_table_remove(&frame->local_table);
                free(frame);
                stack_pop(*stack);
            }
        }
        free((*stack)->data);
        free(*stack);
    }
    *stack = NULL;
}
