#ifndef __TREE_PRINT_H__
#define __TREE_PRINT_H__

#include "def.h"
#include "analyser.h"
#include "generator.h"

void tree_print( Node*, int );
void table_print( Symbol*, int );
void schema_print( Schema* );
void code_print(); // FIXME Why doesn't "Code" work as a parameter??

#endif

