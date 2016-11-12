#ifndef SYMBOL_H
#define SYMBOL_H

#include "token.h"
#include "token_vector.h"
#include "ial.h"

/**
  * @brief Search for variable with specific name and data type.
  *
  * @param iden variable identifier
  * @param local_tab local table of function, body of actual class
  * @param actual_class body of actual class
  * @param dtype data type
  * @return 0 in success or specific error
  */
int find_var(const char *iden, T_symbol_table *local_tab,
             T_symbol *actual_class, T_data_types dtype);

int fill_ifj16();
void remove_ifj16();

#endif
