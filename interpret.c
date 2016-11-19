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
ilist *glist;

T_symbol *get_var(T_symbol *var)
{{{
    T_symbol *ret_var;
    // first search in active frame - local table
    T_frame *frame = stack_top(frame_stack);
    ret_var = table_find(frame->local_tab, var->id, NULL);
    // local variable or static/constant variable returned
    return ret_var ? ret_var : var;
}}}


void interpret_loop(ilist *instr_list)
{{{

    // finding a register
    T_symbol *acc = table_find_simple(symbol_tab, "|accumulator|", 0);
    acc->attr.var->data_type = is_void;
    T_symbol *op1, *op2, *dest;

    T_instr *ins = instr_list->first;
    while (true) {

        if (ins == NULL) {
            if (act_frame->dtype == is_void) {
                // end of run()
                // in frame stack ought to be only run local table
                if (frame_stack->used == 0) {
                    puts("end of run()");
                    return;
                }
                // jump out of function
                // label ought to be in stack pointer
                ins = stack_top(main_stack);
                stack_pop(main_stack);
                remove_frame_from_stack(frame_stack);
            }
            else {
                // no return at function of non void return type
                terminate(8);
            }
        }

        print_instr(ins);

        switch (ins->itype) {
            case TI_mov:
                dest = get_var(ins->dest);
                op1 = get_var(ins->op1);
                // uninitialized or assign from void funtion
                if (op1->attr.var->data_type == is_void ||
                    !op1->attr.var->is_init)
                {
                    terminate(8);
                }
                copy_value(dest, op1);
                break;

            case TI_print:
                // op1 is a constant
                op1 = ins->op1;
                for (int i = 0;i < op1->attr.var->value.n;i++) {
                    T_symbol *out = stack_top(main_stack);
                    out = get_var(out);
                    if (out->attr.var->is_init == false)
                        terminate(8);
                    // XXX print it whole once
                    if (out->attr.var->data_type == is_int) {
                        printf("%d", out->attr.var->value.n);
                    }
                    else if (out->attr.var->data_type == is_double) {
                        printf("%g", out->attr.var->value.d);
                    }
                    else {
                        printf("%s", out->attr.var->value.str);
                    }
                    stack_pop(main_stack);
                }
                break;

            case TI_readInt:
            case TI_readDouble:
            case TI_readString:
                // TODO read character after character till \n, EOF
                break;

            case TI_length:
                op1 = get_var(ins->op1);
                dest = get_var(ins->dest);
                if (dest->attr.var->data_type == is_int)
                    dest->attr.var->value.n = strlen(op1->attr.var->value.str);
                else
                    dest->attr.var->value.d = strlen(op1->attr.var->value.str);
                dest->attr.var->is_init = true;
                break;

            case TI_sort:
            case TI_find:
            case TI_compare:
            case TI_substr:
                // TODO
                break;
            case TI_push:
                stack_push(main_stack, ins->op1);
                break;
            case TI_push_var:
                stack_push(main_stack, get_var(ins->op1));
                break;

            case TI_call:
                create_frame(ins->op1, frame_stack);
                ins = ((T_symbol*)(ins->op1))->attr.func->func_ilist->first;
                continue;

            case TI_jmp:
                ins = ins->op1;
                break;

            case TI_jmpz:
                // check accumulator where result is stored
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
                remove_frame_from_stack(frame_stack);
                // return value already in acc
                break;

            default:
                break;
        }
        // next instruction
        ins = ins->next;
    }

}}}

void interpret(T_symbol *run)
{{{
    frame_stack = stack_init();
    main_stack = stack_init();
    // creating frame for main function run()
    create_frame(run, frame_stack);
//    interpret_loop(glist);
    interpret_loop(run->attr.func->func_ilist);

    // removing stacks
    stack_remove(&frame_stack, true);
    stack_remove(&main_stack, false);
}}}
