#include "generator.h"

extern FILE* yyin;
extern Node* root;
extern Symbol* symbol_table;

FILE* yyin;

stacklist scope;

int yygen( FILE* input, FILE* output )
{
	yyin = input;

	int sem_result = yysem();
	if( sem_result != 0 )
		return sem_result;

	return code_generation( root ) ? 0 : 1;
}

Boolean code_generation( Node* node )
{
	return TRUE;
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
	return 0;
}
