#include <stdio.h>

#ifdef LEXER

#include "parser.h"
#include "hashmap.h"

extern int yylex();
extern char* yytext;
extern map_t lex_symbol_table;

int lex_printf( any_t base, any_t value )
{
  printf( base, value );
  return MAP_OK;
}

int main()
{
    int result;
	do
	{
		result = yylex();
	} while (result > 0);

    printf( "*** Lexical Symbol Table ***\n" );
    hashmap_iterate( lex_symbol_table, lex_printf, "%s\n" );

	return 0;
}

#else
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
#ifdef ANALYZER

#include "analyser.h"

int main()
{
	yysem();

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
#endif
