

#include "globals.h"
#include "stack.h"

#define INIT_SIZE   128
#define GROWTH      8

T_stack *stack_init() {
    T_stack *stack = alloc(sizeof(T_stack));
    stack->data = alloc(INIT_SIZE*sizeof(void*));
    stack->used = 0;
    stack->size = INIT_SIZE - 1;
    return stack;
}

void stack_push(T_stack *stack, void *item) {
    if (stack->used == stack->size) {
        void *p = ralloc(stack->data, stack->size + GROWTH);
        stack->data = p;
        stack->size+=GROWTH;
    }
    stack->data[stack->used++] = item;
}

void *stack_top(T_stack *stack) {
    return stack->data[stack->used];
}

void stack_pop(T_stack *stack) {
    if (stack->used)
        stack->data[stack->used--] = NULL;
}

