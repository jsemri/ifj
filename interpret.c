#include "interpret.h"
#include "instruction.h"
#include "stack.h"
#include "globals.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>
#include "ilist.h"
#include "builtins.h"
#include "debug.h"

#define act_frame ((T_frame*)(stack_top(frame_stack)))
#define is_real(s) (s->attr.var->data_type == is_double)
#define is_integer(s) (s->attr.var->data_type == is_int)
#define is_init(s) (s->attr.var->initialized)

T_stack *frame_stack;
T_stack *main_stack;
T_symbol *acc;
ilist *glist;

T_symbol *get_var(T_symbol *var)
{{{
    if (!var)
        return NULL;

    // if constant
    if (var->attr.var->is_const)
        return var;

    T_symbol *ret_var;
    // first search in active frame - local table
    T_frame *frame = stack_top(frame_stack);
    ret_var = table_find(frame->local_tab, var->id, NULL);
    // local variable or static variable returned
    return ret_var ? ret_var : var;
}}}

void math_instr(T_instr_type itype, T_symbol *dest, T_symbol *op1, T_symbol *op2)
{{{
    // check if both are initialized
    if (!is_init(op1) || !is_init(op2)) {
        puts("8 by math instr");
        terminate(8);
    }

    double x, y;
    if (is_real(op1))
        x = op1->attr.var->value.d;
    else
        x = op1->attr.var->value.n;

    if (is_real(op2))
        y = op1->attr.var->value.d;
    else
        y = op1->attr.var->value.n;


    switch (itype) {
        case TI_add:
            if (is_real(dest))
                dest->attr.var->value.d = x + y;
            else
                dest->attr.var->value.n = (int)(x + y);
            break;

        case TI_sub:
            if (is_real(dest))
                dest->attr.var->value.d = x + y;
            else
                dest->attr.var->value.n = (int)(x - y);
            break;

        case TI_mul:
            if (is_real(dest))
                dest->attr.var->value.d = x * y;
            else
                dest->attr.var->value.n = (int)(x * y);
            break;

        default:
            if (is_real(dest))
                dest->attr.var->value.d = x / y;
            else
                dest->attr.var->value.n = (int)(x / y);
            break;
    }
    dest->attr.var->initialized = true;
}}}


void compare_instr(T_instr_type itype, T_symbol *op1, T_symbol *op2)
{{{
    // check if both initialized
    if (!is_init(op1) || !is_init(op2)) {
        puts("8 by compare instr");
        terminate(8);
    }

    int res;
    double x, y;
    if (is_real(op1))
        x = op1->attr.var->value.d;
    else
        y = op1->attr.var->value.n;

    if (is_real(op2))
        x = op1->attr.var->value.d;
    else
        y = op1->attr.var->value.n;

    switch (itype) {

        case TI_equal:
            res = (x == y);
            break;

        case TI_notequal:
            res = (x != y);
            break;

        case TI_less:
            res = (x < y);
            break;

        case TI_lessEq:
            res = (x <= y);
            break;

        case TI_greater:
            res = (x > y);
            break;

        default:
            res = (x >= y);
            break;
    }
    // logic value stored only in accumulator
    acc->attr.var->data_type = is_int;
    acc->attr.var->value.n = res;
}}}

void interpret_loop(ilist *instr_list)
{{{

    // finding a register
    acc->attr.var->data_type = is_void;
    T_symbol *op1, *dest;
    T_data_type dtype;

    T_instr *ins = instr_list->first;
    while (ins) {print_instr(ins);ins = ins->next;}
    while (false) {

        if (ins == NULL) {
            if (act_frame->dtype == is_void) {
                // end of run()
                // in frame stack ought to be only run local table
                if (frame_stack->used == 0) {
                   // puts("end of run()");
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
                puts("8 by no return at end of non void fnc");
                terminate(8);
            }
        }

        print_instr(ins);


        switch (ins->itype) {
            case TI_add:
            case TI_sub:
            case TI_mul:
            case TI_div:
                math_instr(ins->itype, get_var(ins->dest), get_var(ins->op1),
                         get_var(ins->op2));
                break;
            case TI_concat:
                concat(get_var(ins->dest), get_var(ins->op1), get_var(ins->op2));
                break;
            case TI_equal:
            case TI_notequal:
            case TI_less:
            case TI_lessEq:
            case TI_greater:
            case TI_greaterEq:
                compare_instr(ins->itype, get_var(ins->op1), get_var(ins->op2));
                break;
            case TI_mov:
                dest = get_var(ins->dest);
                op1 = get_var(ins->op1);
                // uninitialized or assign from void funtion
                if (op1->attr.var->data_type == is_void ||
                    !is_init(op1))
                {
                    puts("8 by mov");
                    terminate(8);
                }

                if (is_real(dest)) {
                    double x = is_real(op1) ? op1->attr.var->value.d :
                                              op1->attr.var->value.n;
                    dest->attr.var->value.d = x;
                }
                else if (is_integer(dest)) {
                    double x = is_real(op1) ? op1->attr.var->value.d :
                                              op1->attr.var->value.n;
                    dest->attr.var->value.n = x;
                }
                else {
                   clear_buffer(dest);
                   dest->attr.var->value.str = get_str(op1->attr.var->value.str);
                }
                // TODO
                // copy_value(dest, op1);
                break;

            case TI_print:
                // op1 is a constant
                op1 = ins->op1;
                for (int i = 0;i < op1->attr.var->value.n;i++) {
                    T_symbol *out = stack_top(main_stack);
                    out = get_var(out);
                    if (!is_init(out)) {
                        puts("8 from print");
                        terminate(8);
                    }
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
                dtype = ins->itype == TI_readInt ? is_int : is_str;
                dtype = ins->itype == TI_readDouble ? is_double : dtype;
                read_stdin(get_var(ins->dest), dtype);
                break;

            case TI_length:
                length(get_var(ins->op1), get_var(ins->dest));
                break;

            case TI_sort:
                // TODO
                break;
            case TI_find:
                // TODO
                break;
            case TI_compare:
                compare(get_var(ins->op1), get_var(ins->op2), get_var(ins->dest));
                break;
            case TI_substr:
                op1 = stack_top(main_stack);
                // last parameter on stack
                stack_pop(main_stack);
                substr(get_var(ins->op1), get_var(ins->op2), get_var(op1),
                       get_var(ins->dest));
                break;
            case TI_push:
                stack_push(main_stack, ins->op1);
                break;
            case TI_push_var:
                stack_push(main_stack, get_var(ins->op1));
                break;

            case TI_call:
                create_frame(ins->op1);
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
                    puts("8 by return");
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
    create_frame(run);
//    interpret_loop(glist);
    interpret_loop(run->attr.func->func_ilist);


    // removing stacks
    stack_remove(&frame_stack, true);
    stack_remove(&main_stack, false);
}}}
