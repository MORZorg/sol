#include <stdio.h>

#ifdef PARSER

#include "def.h"
#include "tree_print.h"

extern FILE *yyin;
extern int yyparse();
extern Node* root;

int main()
{
	yyin = stdin;

	int result;

	if ( (result = yyparse()) == 0 )
	{
		printf( "*** Abstract Tree ***\n" );
		tree_print( root, 0 );
	}

	return result;
}

#else
#ifdef LEXER

#include "parser.h"

extern int yylex();
extern char* yytext;

int main()
{
	do
	{
		yylex();
	} while (1);

	return 0;
}

#else

#include <stdlib.h>

int main()
{
	fprintf(stderr, "No body supplied. Exiting.");
	exit(1);
}

#endif
#endif
