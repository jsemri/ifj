#include "globals.h"
#include "stack.h"
#include <stdbool.h>
#include <stdlib.h>
#include "symbol.h"
#include <stdio.h>
#include "debug.h"

#define INIT_SIZE   64
#define GROWTH      2

#define db puts("abcd")

T_stack *main_stack;
T_stack *frame_stack;

T_stack *stack_init() {
    T_stack *stack = calloc(1, sizeof(T_stack));
    stack->data = calloc(INIT_SIZE, sizeof(void*));
    stack->used = -1;
    stack->size = INIT_SIZE - 1;
    return stack;
}

void stack_push(T_stack *stack, void *item) {
    if (stack->used + 2 == stack->size) {
        void *p = realloc(stack->data,sizeof(void*)*stack->size*GROWTH);
        if (!p)
            terminate(INTERNAL_ERROR);
        stack->data = p;
        stack->size*=GROWTH;
    }
    stack->data[++stack->used] = item;
}

void *stack_top(T_stack *stack) {
    return stack->data[stack->used];
}

void stack_pop(T_stack *stack) {
    if (stack->used > -1)
        stack->data[stack->used--] = NULL;
}

bool is_empty(T_stack *stack) {
    return (stack->used == -1);
}

void stack_remove(T_stack **stack, bool is_frame_stack) {
    if (*stack) {
        if (is_frame_stack) {
            while (!is_empty(*stack)) {
                // remove frame
                T_frame *frame = stack_top(*stack);
                remove_frame(&frame);
                stack_pop(*stack);
            }
        }
        free((*stack)->data);
        free(*stack);
    }
    *stack = NULL;
}

void copy_value(T_symbol *dst, T_symbol *src) {
    T_var_symbol *v1 = dst->attr.var;
    T_var_symbol *v2 = src->attr.var;
    // uninitialized value or return value from void function
    if (!v2->initialized || v2->data_type == is_void) {
        puts("uninitialized parameter");
        terminate(8);
    }
    switch (v1->data_type) {
        case is_int:
            v1->value.n = v1->data_type == is_double ?
                          v2->value.d : v2->value.n;
            break;
        case is_double:
            v1->value.d = v1->data_type == is_double ?
                          v2->value.d : v2->value.n;
            break;
        case is_str:
            if (v1->value.str)
                free(v1->value.str);
            v1->value.str = get_str(v2->value.str);
            break;
        case is_void:
            puts("void parameter");
            terminate(8);
        case is_bool:
            v1->value.b = v2->value.b;
            break;
    }
    // raising initialization flag
    v1->initialized = true;
}

void create_frame(T_symbol *func) {
    // creating frame
    T_frame *frame = calloc(1, sizeof(T_frame));
    if (!frame)
        terminate(INTERNAL_ERROR);
    frame->dtype = func->attr.func->data_type;
    frame->local_tab = table_init(RANGE);

    if (!is_empty(main_stack)) {
        for (unsigned i = 0;i < func->attr.func->par_count;i++) {
            copy_value(func->attr.func->arguments[i], stack_top(main_stack));
            stack_pop(main_stack);
        }
    }
    for (unsigned i = 0;i < RANGE;i++) {
        for (T_symbol *sym = func->attr.func->local_table->arr[i];
             sym != NULL; sym = sym->next)
        {
            table_insert(frame->local_tab, symbol_copy(sym));
        }
    }
    stack_push(frame_stack, frame);
}

void remove_frame(T_frame **frame) {
    local_table_remove(&((*frame)->local_tab));
    free((*frame));
    frame = NULL;
}

void remove_frame_from_stack() {
    T_frame *frame = stack_top(frame_stack);
    remove_frame(&frame);
    stack_pop(frame_stack);
}

