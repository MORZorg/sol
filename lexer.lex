%{
#include "def.h"
#include "parser.h"
int line = 1;
Value lexval;
static char* strbuf;

int yyerror(char * type);
%}
%option noyywrap

alpha			[a-zA-Z]
digit			[0-9]

id				{alpha}({alpha}|{digit}|_)*

charconst		'([^\']|\\.)'
intconst		{digit}+
realconst		{digit}+\.{digit}+
boolconst		true|false

spacing			([ \t])+
sugar			[()\[\]{}.,;]

%x charconst
%x strconst

%%
\n					{ fprintf( stderr, "\n" ); line++; }
{spacing}			;
func				{ fprintf( stderr, "%s ", "FUNC" ); return( FUNC ); }
char				{ fprintf( stderr, "%s ", "CHAR" ); return( CHAR ); }
int					{ fprintf( stderr, "%s ", "INT" ); return( INT ); }
real				{ fprintf( stderr, "%s ", "REAL" ); return( REAL ); }
string				{ fprintf( stderr, "%s ", "STRING" ); return( STRING ); }
bool				{ fprintf( stderr, "%s ", "BOOL" ); return( BOOL ); }
struct				{ fprintf( stderr, "%s ", "STRUCT" ); return( STRUCT ); }
vector				{ fprintf( stderr, "%s ", "VECTOR" ); return( VECTOR ); }
of					{ fprintf( stderr, "%s ", "OF" ); return( OF ); }
type				{ fprintf( stderr, "%s ", "TYPE" ); return( TYPE ); }
var					{ fprintf( stderr, "%s ", "VAR" ); return( VAR ); }
const				{ fprintf( stderr, "%s ", "CONST" ); return( CONST ); }
begin				{ fprintf( stderr, "%s ", "SOL_BEGIN" ); return( SOL_BEGIN ); }
end					{ fprintf( stderr, "%s ", "END" ); return( END ); }
if					{ fprintf( stderr, "%s ", "IF" ); return( IF ); }
then				{ fprintf( stderr, "%s ", "THEN" ); return( THEN ); }
endif				{ fprintf( stderr, "%s ", "ENDIF" ); return( ENDIF ); }
elsif				{ fprintf( stderr, "%s ", "ELSIF" ); return( ELSIF ); }
else				{ fprintf( stderr, "%s ", "ELSE" ); return( ELSE ); }
while				{ fprintf( stderr, "%s ", "WHILE" ); return( WHILE ); }
do					{ fprintf( stderr, "%s ", "DO" ); return( DO ); }
endwhile			{ fprintf( stderr, "%s ", "ENDWHILE" ); return( ENDWHILE ); }
for					{ fprintf( stderr, "%s ", "FOR" ); return( FOR ); }
to					{ fprintf( stderr, "%s ", "TO" ); return( TO ); }
endfor				{ fprintf( stderr, "%s ", "ENDFOR" ); return( ENDFOR ); }
foreach				{ fprintf( stderr, "%s ", "FOREACH" ); return( FOREACH ); }
endforeach			{ fprintf( stderr, "%s ", "ENDFOREACH" ); return( ENDFOREACH ); }
return				{ fprintf( stderr, "%s ", "RETURN" ); return( RETURN ); }
read				{ fprintf( stderr, "%s ", "READ" ); return( READ ); }
write				{ fprintf( stderr, "%s ", "WRITE" ); return( WRITE ); }
and					{ fprintf( stderr, "%s ", "AND" ); return( AND ); }
or					{ fprintf( stderr, "%s ", "OR" ); return( OR ); }
in					{ fprintf( stderr, "%s ", "IN" ); return( IN ); }
not					{ fprintf( stderr, "%s ", "NOT" ); return( NOT ); }
toint				{ fprintf( stderr, "%s ", "TOINT" ); return( TOINT ); }
toreal				{ fprintf( stderr, "%s ", "TOREAL" ); return( TOREAL ); }
rd					{ fprintf( stderr, "%s ", "RD" ); return( RD ); }
wr					{ fprintf( stderr, "%s ", "WR" ); return( WR ); }
":"					{ fprintf( stderr, "%s ", "DEFINE" ); return( DEFINE ); }
"="					{ fprintf( stderr, "%s ", "ASSIGN" ); return( ASSIGN ); }
"=="				{ fprintf( stderr, "%s ", "EQ" ); return( EQ ); }
"!="				{ fprintf( stderr, "%s ", "NEQ" ); return( NEQ ); }
">"					{ fprintf( stderr, "%s ", "GT" ); return( GT ); }
">="				{ fprintf( stderr, "%s ", "GEQ" ); return( GEQ ); }
"<"					{ fprintf( stderr, "%s ", "LT" ); return( LT ); }
"<="				{ fprintf( stderr, "%s ", "LEQ" ); return( LEQ ); }
"+"					{ fprintf( stderr, "%s ", "PLUS" ); return( PLUS ); }
"-"					{ fprintf( stderr, "%s ", "MINUS" ); return( MINUS ); }
"*"					{ fprintf( stderr, "%s ", "MULTIPLY" ); return( MULTIPLY ); }
"/"					{ fprintf( stderr, "%s ", "DIVIDE" ); return( DIVIDE ); }
{intconst}			{ fprintf( stderr, "%s ", "INT_CONST" ); lexval.i_val = atoi( yytext ); return( INT_CONST ); }
"\""	           	{ BEGIN strconst; strbuf = new_string( "" ); }
<strconst>[^\\"\n]*	{ concatenate_string( strbuf, yytext ); }
<strconst>\\.		{ char temp[2]; temp[ 0 ] = parse_escape_seq( yytext ); temp[ 1 ] = '\0'; concatenate_string( strbuf, temp ); }
<strconst>\"		{ lexval.s_val = new_string( strbuf ); BEGIN 0; return STR_CONST; }
{charconst}			{ fprintf( stderr, "%s ", "CHAR_CONST" ); lexval.c_val = parse_escape_seq( yytext + 1 ); return( CHAR_CONST ); }
{realconst}			{ fprintf( stderr, "%s ", "REAL_CONST" ); lexval.r_val = atof( yytext ); return( REAL_CONST ); }
{boolconst}			{ fprintf( stderr, "%s ", "BOOL_CONST" ); lexval.b_val = ( yytext[ 0 ] == 'f' ? FALSE : TRUE ); return( BOOL_CONST ); }
{id}				{ fprintf( stderr, "%s ", "ID" ); lexval.s_val = new_string( yytext ); return( ID ); }
{sugar}				{ fprintf( stderr, "%c ", yytext[ 0 ] ); return( yytext[ 0 ] ); }
.					{ yyerror("lexical error"); }
%%

char* new_string( char* s )
{
	char *p;

	p = malloc( sizeof( strlen( s ) + 1 ) );
	strcpy( p, s );
	
	return p;
}

void concatenate_string( char* base, char* addition )
{
	realloc( base, sizeof( strlen( base ) + strlen( addition ) + 1 ) );
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
  fprintf(stderr, "Line %d: %s on symbol \"%s\".\n", line, type, yytext);
  exit(2);
}
