%{
#include "def.h"
#include "parser.h"

char* STR_ERROR = "lexical error";

int line = 1;
Value lexval;
static char* strbuf;

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
intconst		-?{digit}+
realconst		-?{digit}+\.{digit}+

spacing			([ \t])+
sugar			[()\[\]{}.,;]

%x charconst
%x strconst

%%
EOF                 { return 0; }
\n					{ SPAM( "\n" ); line++; }
{spacing}			;
SCODE				{ SPAM( "SCODE\n" ); return( SCODE ); };
NEW					{ SPAM( "NEW\n" ); lexval.op_val = SOL_NEW; return( OP ); }
NEWS				{ SPAM( "NEWS\n" ); lexval.op_val = SOL_NEWS; return( OP ); }
LDC					{ SPAM( "LDC\n" ); lexval.op_val = SOL_LDC; return( OP ); }
LDI					{ SPAM( "LDI\n" ); lexval.op_val = SOL_LDI; return( OP ); }
LDR					{ SPAM( "LDR\n" ); lexval.op_val = SOL_LDR; return( OP ); }
LDS					{ SPAM( "LDS\n" ); lexval.op_val = SOL_LDS; return( OP ); }
LOD					{ SPAM( "LOD\n" ); lexval.op_val = SOL_LOD; return( OP ); }
CAT					{ SPAM( "CAT\n" ); lexval.op_val = SOL_CAT; return( OP ); }
LDA					{ SPAM( "LDA\n" ); lexval.op_val = SOL_LDA; return( OP ); }
FDA					{ SPAM( "FDA\n" ); lexval.op_val = SOL_FDA; return( OP ); }
EIL					{ SPAM( "EIL\n" ); lexval.op_val = SOL_EIL; return( OP ); }
SIL					{ SPAM( "SIL\n" ); lexval.op_val = SOL_SIL; return( OP ); }
IXA					{ SPAM( "IXA\n" ); lexval.op_val = SOL_IXA; return( OP ); }
STO					{ SPAM( "STO\n" ); lexval.op_val = SOL_STO; return( OP ); }
IST					{ SPAM( "IST\n" ); lexval.op_val = SOL_IST; return( OP ); }
JMF					{ SPAM( "JMF\n" ); lexval.op_val = SOL_JMF; return( OP ); }
JMP					{ SPAM( "JMP\n" ); lexval.op_val = SOL_JMP; return( OP ); }
EQU					{ SPAM( "EQU\n" ); lexval.op_val = SOL_EQU; return( OP ); }
NEQ					{ SPAM( "NEQ\n" ); lexval.op_val = SOL_NEQ; return( OP ); }
CGT					{ SPAM( "CGT\n" ); lexval.op_val = SOL_CGT; return( OP ); }
CGE					{ SPAM( "CGE\n" ); lexval.op_val = SOL_CGE; return( OP ); }
CLT					{ SPAM( "CLT\n" ); lexval.op_val = SOL_CLT; return( OP ); }
CLE					{ SPAM( "CLE\n" ); lexval.op_val = SOL_CLE; return( OP ); }
IGT					{ SPAM( "IGT\n" ); lexval.op_val = SOL_IGT; return( OP ); }
IGE					{ SPAM( "IGE\n" ); lexval.op_val = SOL_IGE; return( OP ); }
ILT					{ SPAM( "ILT\n" ); lexval.op_val = SOL_ILT; return( OP ); }
ILE					{ SPAM( "ILE\n" ); lexval.op_val = SOL_ILE; return( OP ); }
RGT					{ SPAM( "RGT\n" ); lexval.op_val = SOL_RGT; return( OP ); }
RGE					{ SPAM( "RGE\n" ); lexval.op_val = SOL_RGE; return( OP ); }
RLT					{ SPAM( "RLT\n" ); lexval.op_val = SOL_RLT; return( OP ); }
RLE					{ SPAM( "RLE\n" ); lexval.op_val = SOL_RLE; return( OP ); }
SGT					{ SPAM( "SGT\n" ); lexval.op_val = SOL_SGT; return( OP ); }
SGE					{ SPAM( "SGE\n" ); lexval.op_val = SOL_SGE; return( OP ); }
SLT					{ SPAM( "SLT\n" ); lexval.op_val = SOL_SLT; return( OP ); }
SLE					{ SPAM( "SLE\n" ); lexval.op_val = SOL_SLE; return( OP ); }
IN					{ SPAM( "IN\n" ); lexval.op_val = SOL_IN; return( OP ); }
IPLUS				{ SPAM( "IPLUS\n" ); lexval.op_val = SOL_IPLUS; return( OP ); }
IMINUS				{ SPAM( "IMINUS\n" ); lexval.op_val = SOL_IMINUS; return( OP ); }
ITIMES				{ SPAM( "ITIMES\n" ); lexval.op_val = SOL_ITIMES; return( OP ); }
IDIV				{ SPAM( "IDIV\n" ); lexval.op_val = SOL_IDIV; return( OP ); }
RPLUS				{ SPAM( "RPLUS\n" ); lexval.op_val = SOL_RPLUS; return( OP ); }
RMINUS				{ SPAM( "RMINUS\n" ); lexval.op_val = SOL_RMINUS; return( OP ); }
RTIMES				{ SPAM( "RTIMES\n" ); lexval.op_val = SOL_RTIMES; return( OP ); }
RDIV				{ SPAM( "RDIV\n" ); lexval.op_val = SOL_RDIV; return( OP ); }
IUMI				{ SPAM( "IUMI\n" ); lexval.op_val = SOL_IUMI; return( OP ); }
RUMI				{ SPAM( "RUMI\n" ); lexval.op_val = SOL_RUMI; return( OP ); }
NEG					{ SPAM( "NEG\n" ); lexval.op_val = SOL_NEG; return( OP ); }
WR					{ SPAM( "WR\n" ); lexval.op_val = SOL_WR; return( OP ); }
FWR					{ SPAM( "FWR\n" ); lexval.op_val = SOL_FWR; return( OP ); }
PUSH				{ SPAM( "PUSH\n" ); lexval.op_val = SOL_PUSH; return( OP ); }
GOTO				{ SPAM( "GOTO\n" ); lexval.op_val = SOL_GOTO; return( OP ); }
POP					{ SPAM( "POP\n" ); lexval.op_val = SOL_POP; return( OP ); }
RD					{ SPAM( "RD\n" ); lexval.op_val = SOL_RD; return( OP ); }
FRD					{ SPAM( "FRD\n" ); lexval.op_val = SOL_FRD; return( OP ); }
TOINT				{ SPAM( "TOINT\n" ); lexval.op_val = SOL_TOINT; return( OP ); }
TOREAL				{ SPAM( "TOREAL\n" ); lexval.op_val = SOL_TOREAL; return( OP ); }
READ				{ SPAM( "READ\n" ); lexval.op_val = SOL_READ; return( OP ); }
FREAD				{ SPAM( "FREAD\n" ); lexval.op_val = SOL_FREAD; return( OP ); }
WRITE				{ SPAM( "WRITE\n" ); lexval.op_val = SOL_WRITE; return( OP ); }
FWRITE				{ SPAM( "FWRITE\n" ); lexval.op_val = SOL_FWRITE; return( OP ); }
FUNC				{ SPAM( "FUNC\n" ); lexval.op_val = SOL_FUNC; return( OP ); }
RETURN				{ SPAM( "RETURN\n" ); lexval.op_val = SOL_RETURN; return( OP ); }
HALT				{ SPAM( "HALT\n" ); lexval.op_val = SOL_HALT; return( OP ); }

{intconst}			{ SPAM( "INT_CONST" ); lexval.i_val = atoi( yytext ); return( CONST ); }
{realconst}			{ SPAM( "REAL_CONST" ); lexval.r_val = atof( yytext ); return( CONST ); }
"\""	           	{ BEGIN strconst; strbuf = malloc( sizeof( char ) ); }
<strconst>[^\\"\n]*	{ concatenate_string( &strbuf, yytext ); }
<strconst>\\.		{ char temp[] = { parse_escape_seq( yytext ), '\0' }; concatenate_string( &strbuf, temp ); }
<strconst>\"		{ lexval.s_val = new_string( strbuf ); BEGIN 0; SPAM( "STR_CONST" ); return CONST; }
{charconst}			{ SPAM( "CHAR_CONST" ); lexval.c_val = parse_escape_seq( yytext + 1 ); return( CONST ); }
{sugar}				{ SPAM( yytext ); return( yytext[ 0 ] ); }
.					{ yyerror( STR_ERROR ); }
%%

char* new_string( char* s )
{
	char* result;

	result = malloc( ( strlen( s ) + 1 ) * sizeof( char ) );
	sprintf( result, "%s", s );

	return result;
}

void concatenate_string( char** base, char* addition )
{
	*base = realloc( *base, sizeof( char ) * ( strlen( *base ) + strlen( addition ) + 1 ) ); 
	strcat( *base, addition );
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

			default:
				if( s[ 1 ] >= '0' && s[ 1 ] <= '7' )
				{
					result = s[ 1 ] - '0';
					if( s[ 2 ] >= '0' && s[ 2 ] <= '7' )
					{
						result = result * 8 + s[ 2 ] - '0';
						if( s[ 3 ] >= '0' && s[ 3 ] <= '7' )
							result = result * 8 + s[ 2 ] - '0';
					}
				}
				else
				{
					// TODO: Add support for hexadecimal and unicode.
				}
				break;
		}
	else
		result = s[ 0 ];

	return result;
}

int yyerror( char* type )
{
	fprintf( stdout, "Line %d: %s on symbol \"%s\".\n", line, type, yytext );
	exit( 2 );
}
