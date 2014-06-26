/*
 * File: main.c (old machine.c)
 * Description: Virtual machine to execute the s-code generated from sol
 * Date: 140623
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include <stdio.h>

#ifdef MACHINE

#include "string.h"
#include "def.h"
#include "executor.h"

#define ERROR_UNDEFINED_FLAG "No such flag."

extern FILE* yyin;
extern int yyparse( void );
extern int program_size;

int main( int argc, char** argv )
{
	char* fileInput = NULL;
	FILE* input;
	FILE* console_output = stdout;

	int i = 0;
	while( ++i < argc )
	{
		if( argv[ i ][ 0 ] != '-' )
			fileInput = argv[ i++ ];
		else
		{
			if( argv[ i ][ 1 ] == '-' )
			{
				argv[ i ] += 2;

				if( !strcmp( argv[ i ], "help" ) )
				{
					// TODO print_help();
					fprintf( console_output, "*** HELP ***\n" );
					return 0;
				}
				else
					if( !strcmp( argv[ i ], "input" ) )
						fileInput = argv[ ++i ];
			}
			else
			{
				char* extended_command;
				switch( argv[ i ][ 1 ] )
				{
					case 'h':
						extended_command = "--help";
						break;

					case 'i':
						extended_command = "--input";
						break;

					default:
						fprintf( console_output, ERROR_UNDEFINED_FLAG );
						// TODO print_help();
						exit( 1 );
				}
				argv[ i-- ] = &extended_command[ 0 ];
			}
		}
	}

	if( fileInput == NULL )
	{
		fprintf( console_output, "Reading from stdin!\n" );
		input = stdin;
	}
	else
	{
		fprintf( console_output, "Reading from '%s'!\n", fileInput );
		input = fopen( fileInput, "r" );
	}

	int result;

	fprintf( stdout, "I want to do something! ノ┬─┬ノ ︵ ( \\o°o)\\\n" );

	// Parsing the file that the user asked for.
	yyin = input;
	if( ( result = yyparse() ) == 0 )
	{
		fprintf( stdout, "I have %d code lines to do!\n", program_size );

		result = yyvm();
	}

	return 0;
}

#else
#ifdef PARSER

#include "def.h"
#include "tree_print.h"

extern int yyparse( void );
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
