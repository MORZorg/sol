%{
#include "def.h"
#include "parser.h"
#include "hashmap.h"

char* STR_ERROR = "lexical error";

int line = 1;
Value lexval;
static char* strbuf;
map_t lex_symbol_table;

int yyerror(char * type);

#ifdef DEBUG
#define SPAM(a) fprintf( stderr, " %s", a );
#else
#define SPAM(a) ;
#endif
%}
%option noyywrap

alpha			[a-zA-Z]
digit			[0-9]

id				{alpha}({alpha}|{digit}|_)*

charconst		'([^\']|\\.)'
intconst		{digit}+
realconst		{digit}+\.{digit}+
boolconst		true|false

comment         --.*
spacing			([ \t])+
sugar			[()\[\]{}.,;]

%x charconst
%x strconst

%%
EOF                 { return 0; }
\n					{ SPAM( "\n" ); line++; }
{spacing}			;
{comment}           { SPAM( "COMMENT" ); }
func				{ SPAM( "FUNC" ); return( FUNC ); }
char				{ SPAM( "CHAR" ); return( CHAR ); }
int					{ SPAM( "INT" ); return( INT ); }
real				{ SPAM( "REAL" ); return( REAL ); }
string				{ SPAM( "STRING" ); return( STRING ); }
bool				{ SPAM( "BOOL" ); return( BOOL ); }
struct				{ SPAM( "STRUCT" ); return( STRUCT ); }
vector				{ SPAM( "VECTOR" ); return( VECTOR ); }
of					{ SPAM( "OF" ); return( OF ); }
type				{ SPAM( "TYPE" ); return( TYPE ); }
var					{ SPAM( "VAR" ); return( VAR ); }
const				{ SPAM( "CONST" ); return( CONST ); }
begin				{ SPAM( "BEGIN" ); return( SOL_BEGIN ); }
end					{ SPAM( "END" ); return( END ); }
if					{ SPAM( "IF" ); return( IF ); }
then				{ SPAM( "THEN" ); return( THEN ); }
endif				{ SPAM( "ENDIF" ); return( ENDIF ); }
elsif				{ SPAM( "ELSIF" ); return( ELSIF ); }
else				{ SPAM( "ELSE" ); return( ELSE ); }
while				{ SPAM( "WHILE" ); return( WHILE ); }
do					{ SPAM( "DO" ); return( DO ); }
endwhile			{ SPAM( "ENDWHILE" ); return( ENDWHILE ); }
for					{ SPAM( "FOR" ); return( FOR ); }
to					{ SPAM( "TO" ); return( TO ); }
endfor				{ SPAM( "ENDFOR" ); return( ENDFOR ); }
foreach				{ SPAM( "FOREACH" ); return( FOREACH ); }
endforeach			{ SPAM( "ENDFOREACH" ); return( ENDFOREACH ); }
return				{ SPAM( "RETURN" ); return( RETURN ); }
read				{ SPAM( "READ" ); return( READ ); }
write				{ SPAM( "WRITE" ); return( WRITE ); }
and					{ SPAM( "AND" ); return( AND ); }
or					{ SPAM( "OR" ); return( OR ); }
in					{ SPAM( "IN" ); return( IN ); }
not					{ SPAM( "NOT" ); return( NOT ); }
toint				{ SPAM( "TOINT" ); return( TOINT ); }
toreal				{ SPAM( "TOREAL" ); return( TOREAL ); }
rd					{ SPAM( "RD" ); return( RD ); }
wr					{ SPAM( "WR" ); return( WR ); }
break               { SPAM( "BREAK" ); return( BREAK ); }
":"					{ SPAM( "DEFINE" ); return( DEFINE ); }
"="					{ SPAM( "ASSIGN" ); return( ASSIGN ); }
"=="				{ SPAM( "EQ" ); return( EQ ); }
"!="				{ SPAM( "NEQ" ); return( NEQ ); }
">"					{ SPAM( "GT" ); return( GT ); }
">="				{ SPAM( "GEQ" ); return( GEQ ); }
"<"					{ SPAM( "LT" ); return( LT ); }
"<="				{ SPAM( "LEQ" ); return( LEQ ); }
"+"					{ SPAM( "PLUS" ); return( PLUS ); }
"-"					{ SPAM( "MINUS" ); return( MINUS ); }
"*"					{ SPAM( "MULTIPLY" ); return( MULTIPLY ); }
"/"					{ SPAM( "DIVIDE" ); return( DIVIDE ); }
{intconst}			{ SPAM( "INT_CONST" ); lexval.i_val = atoi( yytext ); return( INT_CONST ); }
"\""	           	{ BEGIN strconst; strbuf = malloc( sizeof( char ) ); }
<strconst>[^\\"\n]*	{ concatenate_string( strbuf, yytext ); }
<strconst>\\.		{ char temp[] = { parse_escape_seq( yytext ), '\0' }; concatenate_string( strbuf, temp ); }
<strconst>\"		{ lexval.s_val = new_string( strbuf ); BEGIN 0; return STR_CONST; }
{charconst}			{ SPAM( "CHAR_CONST" ); lexval.c_val = parse_escape_seq( yytext + 1 ); return( CHAR_CONST ); }
{realconst}			{ SPAM( "REAL_CONST" ); lexval.r_val = atof( yytext ); return( REAL_CONST ); }
{boolconst}			{ SPAM( "BOOL_CONST" ); lexval.b_val = ( yytext[ 0 ] == 'f' ? FALSE : TRUE ); return( BOOL_CONST ); }
{id}				{ SPAM( "ID" ); lexval.s_val = new_string( yytext ); return( ID ); }
{sugar}				{ SPAM( yytext ); return( yytext[ 0 ] ); }
.					{ yyerror( STR_ERROR ); }
%%

char* new_string( char* s )
{
    if( lex_symbol_table == NULL )
      lex_symbol_table = hashmap_new();

	char* result;

    if( hashmap_get( lex_symbol_table, s, (any_t*) &result ) == MAP_MISSING )
    {
      result = malloc( sizeof( strlen( s ) + 1 ) );
      strcpy( result, s );

      hashmap_put( lex_symbol_table, result, result );
    }
	
	return result;
}

void concatenate_string( char* base, char* addition )
{
	base = realloc( base, sizeof( strlen( base ) + strlen( addition ) + 1 ) );
	strcat( base, addition );
}

char parse_escape_seq( char* s )
{
  char result;

  if( s[ 0 ] == '\\' )
    switch( s[ 1 ] )
    {
		case 'a':
			result = '\a';
			break;

		case 'b':
			result = '\b';
			break;

		case 'f':
			result = '\f';
			break;

		case 'n':
			result = '\n';
			break;

		case 'r':
			result = '\r';
			break;

		case 't':
			result = '\t';
			break;

		case 'v':
			result = '\v';
			break;

		case '\\':
			result = '\\';
			break;

		case '\'':
			result = '\'';
			break;

		case '"':
			result = '\"';
			break;

		case '?':
			result = '\?';
			break;

		case '0':
			if( s[ 2 ] == 0 )
				result = '\0';
			else
			break;

		// TODO: Add support for octal, hexadecimal and unicode.
    }
  else
    result = s[ 0 ];

  return result;
}

int yyerror(char* type)
{
  fprintf( stdout, "Line %d: %s on symbol \"%s\".\n", line, type, yytext );
  exit(2);
}
