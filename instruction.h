/*
 * ilist.h
 *
 */

#ifndef INSTR_H
#define INSTR_H

// instruction type
typedef enum {
    TI_nop
    // TODO
    // more instructions
} T_instr_type;

// instruction mode
typedef enum {
    TM_const1,      // first operand is constant
    TM_const2,      // second...
    TM_const_all,   // first and second...
} T_instr_mode;

// instruction
typedef struct T_instruction {
    T_instr_type itype;
    T_instr_mode imode;
    void *op1;                   // first operand
    void *op2;                   // second operand
    void *dest;                  // result destination
    struct T_instruction *next;  // next instruction
} T_instr;

// instruction list
typedef struct T_ilist {
    T_instr *first; // first item
    T_instr *last;  // last item
    T_instr *act;   // active item
} ilist;

// TODO
// add functions

#endif