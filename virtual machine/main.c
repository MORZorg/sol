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
#define SOL_EXTENSION "sol"
#define OHANA_EXTENSION "ohana"
#define GENERATOR_PATH "./solc"

extern FILE* yyin;
extern int yyparse( void );
extern int program_size;

char* get_file_extension( char* );
char* change_file_extension( char*, char*, char* );
char* formatted_command( char* );

int main( int argc, char** argv )
{
	char* fileInput = NULL;
	FILE* input;
	FILE* console_output = stdout;
	int result;
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
		char* extension = get_file_extension( fileInput );
		if( !strcmp( extension, SOL_EXTENSION ) )
		{
			// If I have the sol file, i must compile it before execute it.
            char* formatted_file = formatted_command( fileInput );
			char* terminal_command = malloc( sizeof( char ) * ( strlen( GENERATOR_PATH ) + strlen( formatted_file ) + 2 ) );
			// In the command i have to substitute the single spaces with the formatted spaces
			sprintf( terminal_command, "%s %s", GENERATOR_PATH, formatted_file );
			fprintf( stderr, "Generating code: '%s'\n", terminal_command );
			if( ( result = system( terminal_command ) ) != 0 )
				exit( result );

			// Changing the fileInput to the compiled one.
			fileInput = change_file_extension( fileInput, SOL_EXTENSION, OHANA_EXTENSION );
		}
		fprintf( console_output, "Reading from '%s'!\n", fileInput );
		input = fopen( fileInput, "r" );
	}


	// Parsing the file that the user asked for.
	yyin = input;
	if( ( result = yyparse() ) == 0 )
	{
#ifdef DEBUG
		fprintf( stdout, "I have %d code lines to do!\n", program_size );
#endif

		if( ( result = yyvm() ) == 0 )
			fprintf( stdout, "Executed all S-Code ノ┬─┬ノ ︵ ( \\o°o)\\\n" );
	}

	return result;
}

char* get_file_extension( char* filename )
{
	char* extension = &( filename[ strlen( filename ) ] );
	while( *(extension--) != '.' );
	extension += 2;

	return extension;
}

char* change_file_extension( char* filename, char* old_ext, char* new_ext )
{
    int base_len = strlen( filename ) - strlen( old_ext );
	char* new_filename = malloc( sizeof( char ) * ( base_len + strlen( new_ext ) + 1 ) );

    memcpy( new_filename, filename, sizeof( char ) * base_len );
    strcpy( new_filename + base_len, new_ext ); 

	return new_filename;
}

char* formatted_command( char* command )
{
	// Looking for spaces and substituting with '\ '
	char* iterator = command;
	int spaces = 0;
	while( *(iterator) != '\0' )
	{
		if( *(iterator) == ' ' )
			spaces++;
		iterator++;
	}

	// Creating the new file name
	char* formatted = malloc( sizeof( char ) * ( strlen( command ) + spaces + 1 ) );
	iterator = formatted;

	while( *(command) != '\0' )
	{
		if( *(command) == ' ' )
		{
			*(iterator) = '\\';
			iterator++;
		}
		*(iterator) = *(command);
		command++;
		iterator++;
	}
    *(iterator) = '\0';

	return formatted;
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
