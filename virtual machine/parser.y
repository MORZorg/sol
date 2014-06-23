%{
#include <string.h>
#include <stdarg.h>

#include "def.h"

#define YYSTYPE Value
extern FILE* yyin;
extern char* yytext;
extern Value lexval;
extern int line;

extern int yylex();
extern int yyerror( char* );

Stat* program;
%}

%token SCODE OP CONST

%%

program : scode stat_list
		;

stat_list : stat stat_list
		  | 
		  ;

stat : no_param
	 | one_param
	 | two_param
	 | three_param
	 ;

op : OP { $$ = lexval; }
   ;

const : CONST { $$ = lexval; }
	  ;

scode : SCODE const { program = malloc( sizeof( Stat ) * $2.i_val ); }
	  ;

no_param : op { program[ line - 1 ] = new_stat( 0, $1.op_val ); }
		 ;

one_param : op const { program[ line - 1 ] = new_stat( 1, $1.op_val, $2 ); }
		  ;

two_param : op const const { program[ line - 1 ] = new_stat( 2, $1.op_val, $2, $3 ); }
		  ;

three_param : op const const const { program[ line - 1 ] = new_stat( 3, $1.op_val, $2, $3, $4 ); }
			;

%%

Stat new_stat( int argc, Operator op, ... )
{
	va_list argv;
	va_start( argv, op );

	Stat result;
	result.op = op;

	int i;
	for( i = 0; i < argc; i++ )
		result.args[ i ] = va_arg( argv, Value );

	va_end( argv );

	return result;
}

