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
 * @file instruction.h
 * @brief Contains instructions.
 */

#ifndef INSTR_H
#define INSTR_H


// instruction type
// id - variable
// v - constant
typedef enum {
    TI_mov,         // id id/v -
    TI_add,
    TI_sub,
    TI_mul,
    TI_div,
    TI_concat,      // string concatenation
    TI_equal,
    TI_notequal,
    TI_less,
    TI_lessEq,
    TI_greater,
    TI_greaterEq,
    TI_jmp,         // jumps
    TI_jmpz,        // jump if zero
    TI_ret,
    TI_call,        // id - - | all pars on stack
    TI_push,
    TI_push_var,
    TI_readInt,     // id/- - -
    TI_readDouble,
    TI_readString,
    TI_print,       // count - - | all pars in stack including their count
    TI_length,      // id/- id/v -
    TI_substr,      // id/- id/v id/v id/v | one par on stack
    TI_compare,     // id/- id/v id/v
    TI_find,        // id/- id/v id/v
    TI_sort,        // id/- id/v -
    TI_lab,
} T_instr_type;

/// Structure of instruction.
typedef struct T_instruction {
    T_instr_type itype;          // instruction type
    void *op1;                   // first operand
    void *op2;                   // second operand
    void *dest;                  // result destination
    struct T_instruction *next;  // next instruction
} T_instr;

/// Structure of instruction list.
typedef struct T_ilist {
    T_instr *first; // first item
    T_instr *last;  // last item
} ilist;


/**
 * Creates and inserts instruction to a instruction list.
 *
 * @param L instruction list
 * @param itype instruction type
 * @param op1 first operand
 * @param op2 second operand
 * @param dest destination where result will be stored
 */
void *create_instr( ilist *L, T_instr_type itype, void *op1, void *op2,
                   void *dest);

/**
 * Creates instruction.
 *
 * @param itype instruction type
 * @param op1 first operand
 * @param op2 second operand
 * @param dest destination where result will be stored
 */
T_instr *instr_init(T_instr_type itype, void *op1, void *op2, void *dest);

#endif
