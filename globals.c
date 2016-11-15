#include "globals.h"
#include "token.h"
#include <stdio.h>
#include "ial.h"
#include "instruction.h"
#include "symbol.h"
#include "ilist.h"
#include <stdlib.h>

ilist *instr_list;
T_symbol_table *symbol_tab;
FILE *source;
T_token *token;

void terminate(int err_code) {

    token_free(&token);
    fclose(source);
    list_free(&instr_list);
    table_remove(&symbol_tab);

    exit(err_code);
}


