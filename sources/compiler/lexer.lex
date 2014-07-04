%{
#include "def.h"
#include "parser.h"
#include "../libs/hashmap.h"

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

alpha           [a-zA-Z]
digit           [0-9]

id              {alpha}({alpha}|{digit}|_)*

charconst       '([^\']|\\.)'
intconst        {digit}+
realconst       {digit}+\.{digit}+
boolconst       true|false

comment         --.*
spacing         ([ \t])+
sugar           [()\[\]{}.,;]

%x strconst

%%
EOF                    { return 0; }
\n                     { line++; }
{spacing}              ;
{comment}              { }
func                   { return( FUNC ); }
char                   { return( CHAR ); }
int                    { return( INT ); }
real                   { return( REAL ); }
string                 { return( STRING ); }
bool                   { return( BOOL ); }
struct                 { return( STRUCT ); }
vector                 { return( VECTOR ); }
of                     { return( OF ); }
type                   { return( TYPE ); }
var                    { return( VAR ); }
const                  { return( CONST ); }
begin                  { return( SOL_BEGIN ); }
end                    { return( END ); }
if                     { return( IF ); }
then                   { return( THEN ); }
endif                  { return( ENDIF ); }
elsif                  { return( ELSIF ); }
else                   { return( ELSE ); }
while                  { return( WHILE ); }
do                     { return( DO ); }
endwhile               { return( ENDWHILE ); }
for                    { return( FOR ); }
to                     { return( TO ); }
endfor                 { return( ENDFOR ); }
foreach                { return( FOREACH ); }
endforeach             { return( ENDFOREACH ); }
return                 { return( RETURN ); }
read                   { return( READ ); }
write                  { return( WRITE ); }
and                    { return( AND ); }
or                     { return( OR ); }
in                     { return( IN ); }
not                    { return( NOT ); }
toint                  { return( TOINT ); }
toreal                 { return( TOREAL ); }
rd                     { return( RD ); }
wr                     { return( WR ); }
break                  { return( BREAK ); }
":"                    { return( DEFINE ); }
"="                    { return( ASSIGN ); }
"=="                   { return( EQ ); }
"!="                   { return( NEQ ); }
">"                    { return( GT ); }
">="                   { return( GEQ ); }
"<"                    { return( LT ); }
"<="                   { return( LEQ ); }
"+"                    { return( PLUS ); }
"-"                    { return( MINUS ); }
"*"                    { return( MULTIPLY ); }
"/"                    { return( DIVIDE ); }
{intconst}             { lexval.i_val = atoi( yytext );
                         return( INT_CONST ); }
"\""                   { BEGIN strconst;
                         strbuf = malloc( sizeof( char ) ); }
<strconst>([^"\n])*    { concatenate_string( &strbuf, yytext ); }
<strconst>\n[ \t]*     ;
<strconst>\"           { lexval.s_val = new_string( strbuf );
                         BEGIN 0;
                         return( STR_CONST ); }
{charconst}            { yytext[ strlen( yytext ) - 1 ] = '\0';
                         lexval.s_val = new_string( yytext + 1 );
                         return( CHAR_CONST ); }
{realconst}            { lexval.r_val = atof( yytext );
                         return( REAL_CONST ); }
{boolconst}            { lexval.b_val = ( yytext[ 0 ] == 'f'
                                          ? FALSE
                                          : TRUE );
                         return( BOOL_CONST ); }
{id}                   { lexval.s_val = new_string( yytext );
                         return( ID ); }
{sugar}                { return( yytext[ 0 ] ); }
.                      { yyerror( STR_ERROR ); }
%%

char* new_string( char* s )
{
    if( lex_symbol_table == NULL )
        lex_symbol_table = hashmap_new();

    char* result;

    if( hashmap_get( lex_symbol_table, s, (any_t*) &result ) == MAP_MISSING )
    {
        result = malloc( ( strlen( s ) + 1 ) * sizeof( char ) );
        sprintf( result, "%s", s );

        hashmap_put( lex_symbol_table, result, result );
    }

    return result;
}

void concatenate_string( char** base, char* addition )
{
    *base = realloc( *base, sizeof( char ) * ( strlen( *base ) + strlen( addition ) + 1 ) ); 
    strcat( *base, addition );
}

int yyerror( char* type )
{
    fprintf( stdout, "Line %d: %s on symbol \"%s\".\n", line, type, yytext );
    exit( 2 );
}
