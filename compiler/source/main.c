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
#ifdef ANALYSER

#include "analyser.h"

extern Symbol* symbol_table;

int main()
{
	if( yysem() == SEM_OK )
		table_print( symbol_table, 0 );

	return 0;
}

#else

#include <string.h>

#include "generator.h"

// FIXME Put in a .h file
char* change_extension( char* a_path );

int main( int argc, char** argv )
{
	char* fileInput;
	char* fileOutput;

	int i = 0;
	while (++i < argc)
	{
		if( argv[ i ][ 0 ] != '-' )
			fileInput = argv[ i++ ];
		else if( argv[ i ][ 1 ] == '-' )
		{
			argv[ i ] += 2;

			if( !strcmp( argv[ i ], "help" ) )
			{
				/* TODO printHelp(); */
				return 0;
			}
			else if( !strcmp( argv[ i ], "output" ) )
			{
				fileOutput = argv[ ++i ];
			}
		}
		else
		{
			char* extended_command;
			switch( argv[ i ][ 1 ] )
			{
				case 'h':
					extended_command = "--help";
					argv[ i-- ] = &extended_command[ 0 ];
					break;

				case 'o':
					extended_command = "--output";
					argv[ i-- ] = &extended_command[ 0 ];
					break;

				default:
					fprintf( stderr, ERROR_UNDEFINED_FLAG );
					// TODO
					/* printHelp(); */
					exit( 1 );
			}
		}
	}

	FILE* input;
	FILE* output;
	if( fileInput == NULL )
	{
		fprintf( stderr, "Reading from stdin!\n" );
		input = stdin;
	}
	else
	{
		fprintf( stderr, "Reading from %s!\n", fileInput );
		input = fopen( fileInput, "r" );
	}
	if( fileOutput == NULL )
	{
		if( fileInput == NULL )
		{
			fprintf( stderr, "Printing to stdout!\n" );
			output = stdout;
		}
		else
		{
			fileOutput = change_extension( fileInput );
			output = fopen( fileOutput, "w+" );
			fprintf( stderr, "Printing to %s!\n", fileOutput );
		}
	}
	else
	{
		fprintf( stderr, "Printing to %s!\n", fileOutput );
		output = fopen( fileOutput, "w+" );
	}

	if( yygen( input, output ) == 0 )
		fprintf( stdout, "Generated all S-code (ﾉ◕ヮ◕)ﾉ*:・ﾟ✧\n" ); 

	return 0;
}

char* change_extension( char* a_path )
{
	int base_len;
	for( base_len = strlen( a_path )-1; base_len > 0; base_len-- )
		if( a_path[ base_len ] == '.' )
			break;
		else if( a_path[ base_len ] == '/' )
			break;
	if( base_len != '.' )
		base_len = strlen( a_path );
	else
		base_len--;

	char* result = malloc( ( base_len + strlen( INTERMEDIATE_CODE_EXTENSION ) )
						   * sizeof( char ) );
	// FIXME Could cause problems when the original extension is longer than
	// the new one.
	strcat( result, a_path );
	result[ base_len+1 ] = 0;
	strcat( result, INTERMEDIATE_CODE_EXTENSION );

	return result;
}

#endif
#endif
#endif
