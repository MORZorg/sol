/*
 * File: machine.c
 * Description: Virtual machine to execute the s-code generated from sol
 * Date: 140623
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include "executor.h"
#include "tree_print.h"

extern int yyparse();
extern Stat* program;
extern int program_size;

int main( int argc, char** argv )
{
	// Lex + Yacc produce a vector of Stat*
	// Stat* statements = yysomething();

	// yyvm( statements );
	// Profit.
	
	int result;

	fprintf( stdout, "Something\n" );

	if( ( result = yyparse() ) == 0 )
	{
		fprintf( stdout, "*** Readen code ***\n" );
		code_array_print( program, program_size );
	}

	return 0;
}
