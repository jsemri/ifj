#include "interpret.h"
#include "instruction.h"
#include "stack.h"
#include "globals.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>
#include "ilist.h"
#include "debug.h"

T_stack *frame_stack;
T_stack *main_stack;
ilist *instr_list;

void create_frame(T_symbol *func, T_stack *stack)
{{{
    // creating frame
    T_symbol_table *frame = table_init(RANGE);
    // copying all variables
    for (unsigned i = 0;i < RANGE;i++) {
        for (T_symbol *sym = func->attr.func->local_table->arr[i];
             sym != NULL; sym = sym->next)
        {
            table_insert(frame, symbol_copy(sym));
        }
    }
    // TODO initialize parameters - they ought to be in main stack
    stack_push(stack, frame);
}}}

void remove_frame(T_stack *stack)
{{{
    T_symbol_table *frame = stack_top(stack);
    stack_pop(stack);
    local_table_remove(&frame);
}}}

T_symbol *find_var(T_symbol *var)
{{{
    // constant
    if (var->attr.var->is_const)
        return var;

    T_symbol *ret_var;
    // first search in active frame - local table
    if (frame_stack->used) {
        T_func_symbol *func = stack_top(frame_stack);
        ret_var = table_find(func->local_table, var->id, NULL);
    }
    // found in actual frame
    if (ret_var)
        return ret_var;
    // variable ought to be static, so will be returned
    return var;
}}}

void interpret_loop()
{{{

    T_instr *ins = instr_list->first;
    while (ins != NULL) {

//        print_instr(ins);
        switch (ins->itype) {
            case TI_mov:

            case TI_add:

            default:
                break;
        }
        ins = ins->next;
    }

    return;
}}}

void interpret()
{{{
    frame_stack = stack_init();
    main_stack = stack_init();

    interpret_loop();

    stack_remove(&frame_stack, true);
    stack_remove(&main_stack, false);
}}}
