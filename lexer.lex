%{
#include "def.h"
int line = 1;
Value lexval;
%}
%option noyywrap

alpha			[a-zA-Z]
digit			[0-9]

id				{alpha}({alpha}|{digit}|_)*

charconst		'([^\']|\\.)'
intconst		{digit}+
realconst		{digit}+\.{digit}+
strconst		\"([^\"]|\\\")*\"
boolconst		true|false

spacing			([ \t])+
sugar			[()\[\]{}.,;]

%%
\n				{ printf( "\n" ); line++; }
{spacing}		;
func			{ printf( "%s ", "FUNC" ); return( FUNC ); }
char			{ printf( "%s ", "CHAR" ); return( CHAR ); }
int				{ printf( "%s ", "INT" ); return( INT ); }
real			{ printf( "%s ", "REAL" ); return( REAL ); }
string			{ printf( "%s ", "STRING" ); return( STRING ); }
bool			{ printf( "%s ", "BOOL" ); return( BOOL ); }
struct			{ printf( "%s ", "STRUCT" ); return( STRUCT ); }
vector			{ printf( "%s ", "VECTOR" ); return( VECTOR ); }
of				{ printf( "%s ", "OF" ); return( OF ); }
type			{ printf( "%s ", "TYPE" ); return( TYPE ); }
var				{ printf( "%s ", "VAR" ); return( VAR ); }
const			{ printf( "%s ", "CONST" ); return( CONST ); }
begin			{ printf( "%s ", "SOL_BEGIN" ); return( SOL_BEGIN ); }
end				{ printf( "%s ", "END" ); return( END ); }
if				{ printf( "%s ", "IF" ); return( IF ); }
then			{ printf( "%s ", "THEN" ); return( THEN ); }
endif			{ printf( "%s ", "ENDIF" ); return( ENDIF ); }
elsif			{ printf( "%s ", "ELSIF" ); return( ELSIF ); }
else			{ printf( "%s ", "ELSE" ); return( ELSE ); }
while			{ printf( "%s ", "WHILE" ); return( WHILE ); }
do				{ printf( "%s ", "DO" ); return( DO ); }
endwhile		{ printf( "%s ", "ENDWHILE" ); return( ENDWHILE ); }
for				{ printf( "%s ", "FOR" ); return( FOR ); }
endfor			{ printf( "%s ", "ENDFOR" ); return( ENDFOR ); }
foreach			{ printf( "%s ", "FOREACH" ); return( FOREACH ); }
endforeach		{ printf( "%s ", "ENDFOREACH" ); return( ENDFOREACH ); }
return			{ printf( "%s ", "RETURN" ); return( RETURN ); }
read			{ printf( "%s ", "READ" ); return( READ ); }
write			{ printf( "%s ", "WRITE" ); return( WRITE ); }
and				{ printf( "%s ", "AND" ); return( AND ); }
or				{ printf( "%s ", "OR" ); return( OR ); }
in				{ printf( "%s ", "IN" ); return( IN ); }
not				{ printf( "%s ", "NOT" ); return( NOT ); }
toint			{ printf( "%s ", "TOINT" ); return( TOINT ); }
toreal			{ printf( "%s ", "TOREAL" ); return( TOREAL ); }
rd				{ printf( "%s ", "RD" ); return( RD ); }
wr				{ printf( "%s ", "WR" ); return( WR ); }
":"				{ printf( "%s ", "DEFINE" ); return( DEFINE ); }
"="				{ printf( "%s ", "ASSIGN" ); return( ASSIGN ); }
"=="			{ printf( "%s ", "EQ" ); return( EQ ); }
"!="			{ printf( "%s ", "NEQ" ); return( NEQ ); }
">"				{ printf( "%s ", "GT" ); return( GT ); }
">="			{ printf( "%s ", "GEQ" ); return( GEQ ); }
"<"				{ printf( "%s ", "LT" ); return( LT ); }
"<="			{ printf( "%s ", "LEQ" ); return( LEQ ); }
"+"				{ printf( "%s ", "PLUS" ); return( PLUS ); }
"-"				{ printf( "%s ", "MINUS" ); return( MINUS ); }
"*"				{ printf( "%s ", "MUL" ); return( MUL ); }
"/"				{ printf( "%s ", "DIVIDE" ); return( DIVIDE ); }
{intconst}		{ printf( "%s ", "INTCONST" ); lexval.i_val = atoi( yytext ); return( INTCONST ); }
{charconst}		{ printf( "%s ", "CHARCONST" ); lexval.c_val = yytext[ 0 ]; return( CHARCONST ); }
{realconst}		{ printf( "%s ", "REALCONST" ); lexval.r_val = atof( yytext ); return( REALCONST ); }
{strconst}		{ printf( "%s ", "STRCONST" ); lexval.s_val = new_string( yytext ); return( STRCONST ); }
{boolconst}		{ printf( "%s ", "BOOLCONST" ); lexval.b_val = ( yytext[ 0 ] == 'f' ? FALSE : TRUE ); return( BOOLCONST ); }
{id}			{ printf( "%s ", "ID" ); lexval.s_val = new_string( yytext ); return( ID ); }
{sugar}			{ printf( "%c ", yytext[ 0 ] ); return( yytext[ 0 ] ); }
.				{ printf( "\nError on line %d: %s", line, yytext ); return( ERROR ); }
%%

char* new_string( char* s )
{
	char *p;

	p = malloc( sizeof( strlen( s ) + 1 ) );
	strcpy( p, s );
	
	return p;
}

int main()
{
	int value = 0;
	do
	{
		value = yylex();
	} while (value != ERROR);
}
