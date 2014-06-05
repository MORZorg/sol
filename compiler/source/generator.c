#include "generator.h"

extern Node* root;
extern Symbol* symbol_table;
/* extern int yysem(); */

stacklist scope;

int yygen()
{
}


/**
 * @brief Error function called in case of error while generating code.
 *
 * @param node
 * @param type
 *
 * @return 
 */
int yygenerror( Node* node, char* type )
{
}
