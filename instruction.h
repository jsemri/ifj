/**
 * @file instruction.h
 * @brief Contains instructions.
 */

#ifndef INSTR_H
#define INSTR_H

// instruction type
typedef enum {
    TI_nop,         // no operation
    TI_mov,         // assignment
    TI_add,         // arithmetic
    TI_sub,
    TI_mul,
    TI_div,
    TI_concat,      // string concatenation
    TI_and,         // comparison/relational
    TI_or,
    TI_not,
    TI_equal,
    TI_notequal,
    TI_less,
    TI_lessEq,
    TI_greater,
    TI_greaterEq,
    TI_jmp,         // jumps
    TI_jmpz,        // jump if zero
    TI_return,
    TI_call,        // calling function
    TI_push_params, // pushing parameters to stack
    TI_readInt,     // built-ins
    TI_readDouble,
    TI_readString,
    TI_print,
    TI_length,
    TI_substr,
    TI_compare,
    TI_find,
    TI_sort,
    TI_castIntDouble, // casts an int to double
} T_instr_type;

// instruction mode
typedef enum {
    TM_const1,      // first operand is constant
    TM_const2,      // second...
    TM_const_all,   // first and second...
} T_instr_mode;

/// Structure of instruction.
typedef struct T_instruction {
    T_instr_type itype;          // instruction type
    T_instr_mode imode;          // instruction mode
    void *op1;                   // first operand
    void *op2;                   // second operand
    void *dest;                  // result destination
    struct T_instruction *next;  // next instruction
} T_instr;

/// Structure of instruction list.
typedef struct T_ilist {
    T_instr *first; // first item
    T_instr *last;  // last item
    T_instr *act;   // active item
} ilist;

// TODO Add functions
ilist *list_init();
void list_insert_first(ilist *L, T_instr *ins);
void list_insert_last(ilist *L, T_instr *ins);
void list_delete(ilist *L);
void list_free(ilist **L);

int create_inst( ilist *L, T_instr_type itype, T_instr_mode imode,
                        void *op1, void *op2, void *dest);

#endif
