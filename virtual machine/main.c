/*
 * File: main.c (old machine.c)
 * Description: Virtual machine to execute the s-code generated from sol
 * Date: 140623
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include <stdio.h>

#ifdef MACHINE

#include "def.h"
#include "executor.h"

extern int yyparse();
extern Stat* program;
extern int program_size;

int main( int argc, char** argv )
{
	int result;

	fprintf( stdout, "I want to do something! ノ┬─┬ノ ︵ ( \\o°o)\\\n" );

	if( ( result = yyparse() ) == 0 )
		fprintf( stdout, "I have %d code lines to do!\n", program_size );

	return 0;
}

#else
#ifdef PARSER

#include "def.h"
#include "tree_print.h"

extern int yyparse();
extern Stat* program;
extern int program_size;

int main( int argc, char** argv )
{
	int result;

	if( ( result = yyparse() ) == 0 )
	{
		fprintf( stdout, "*** Read code ***\n" );
		code_array_print( program, program_size );
	}

	return 0;
}

#endif
#endif
