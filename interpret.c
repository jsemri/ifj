#include "interpret.h"
#include "instruction.h"
#include "stack.h"
#include "globals.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>
#include "ilist.h"
#include "debug.h"

#define act_frame ((T_frame*)(stack_top(frame_stack)))

T_stack *frame_stack;
T_stack *main_stack;

T_symbol *get_var(T_symbol *var)
{{{
    // constant or static variable
    if (var->attr.var->is_const || var->member_class)
        return var;

    T_symbol *ret_var;
    // first search in active frame - local table
    T_frame *frame = stack_top(frame_stack);
    ret_var = table_find(frame->local_tab, var->id, NULL);
    // found in actual frame
    return ret_var;
}}}


void interpret_loop(ilist *instr_list)
{{{

    // finding a register
    T_symbol *acc = table_find_simple(symbol_tab, "|accumulator|", 0);
    T_symbol *op1, *op2, *dest;

    T_instr *ins = instr_list->first;
    while (ins != NULL) {

        print_instr(ins);
        switch (ins->itype) {
            case TI_mov:
                dest = get_var(dest);
                op1 = get_var(dest);
                // uninitialized or assign from void funtion
                if (op1->attr.var->data_type == is_void ||
                    !op1->attr.var->is_init)
                {
                    terminate(8);
                }
                // get value
                break;
            case TI_push:
                stack_push(main_stack, op1);
                break;
            case TI_call:
                // TODO
                // create frame
                // copy all content of stack to parameters
                // jump to function
            case TI_jmp:
                ins = ins->op1;
                break;
            case TI_jmpz:
                if (acc)
                    ins = ins->op1;
                break;
            case TI_ret:
                // getting an address of next jump
                if (acc->attr.var->data_type != is_void &&
                    act_frame->dtype == is_void)
                {
                    terminate(8);
                }
                ins = stack_top(main_stack);
                stack_pop(main_stack);
                // return value already in acc
                break;
            default:
                break;
        }
        ins = ins->next;
    }

    return;
}}}

void interpret(T_symbol *run)
{{{
    frame_stack = stack_init();
    main_stack = stack_init();

    // creating frame for main function run()
    create_frame(run, frame_stack);
    interpret_loop(run->attr.func->func_ilist);

    // removing stacks
    stack_remove(&frame_stack, true);
    stack_remove(&main_stack, false);
}}}
