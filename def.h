#include <stdio.h>
#include <stdlib.h>

#define FUNC		259 
#define CHAR		260
#define INT			261
#define REAL		262
#define STRING		263
#define BOOL		264
#define STRUCT		265
#define VECTOR		266
#define OF			267
#define TYPE		268
#define VAR			269
#define CONST		270
#define SOL_BEGIN	271
#define END			272
#define IF			273
#define THEN		274
#define ENDIF		275
#define ELSIF		276
#define ELSE		277
#define WHILE		278
#define DO			279
#define ENDWHILE	280
#define FOR			281
#define ENDFOR		282
#define FOREACH		283
#define ENDFOREACH	284
#define RETURN		285
#define READ		286
#define WRITE		287
#define AND			288
#define OR			289
#define IN			290
#define NOT			291
#define TOINT		292
#define TOREAL		293
#define RD			294
#define WR			295
#define DEFINE		296
#define ASSIGN		297
#define EQ			298 
#define NEQ			299 
#define GT			300 
#define GEQ			301 
#define LT			302 
#define LEQ			303 
#define PLUS		304 
#define MINUS		305 
#define MUL			306 
#define DIVIDE		307 
#define ID			308
#define	INTCONST	309
#define	CHARCONST	310
#define	REALCONST	311
#define	STRCONST	312
#define	BOOLCONST	313
#define ERROR		314 

typedef union
{
	int i_val;
	char* s_val;
    char c_val;
	float r_val;
	enum { FALSE, TRUE } b_val;
} Value;

char* new_string( char* );
