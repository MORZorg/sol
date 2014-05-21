#include "def.h"
#include "hashmap.h"

#ifndef __ANALYZER_H__
#define __ANALYZER_H__

typedef enum
{
	CS_TYPE,
	CS_VAR,
	CS_CONST,
	CS_FUNC,
	CS_PAR
} ClassSymbol;

typedef enum
{
	TS_CHAR,
	TS_INT,
	TS_REAL,
	TS_STRING,
	TS_BOOL,
	TS_STRUCT,
	TS_VECTOR,
	TS_ATTR
} TypeSchema;

typedef struct schema
{
	TypeSchema type;
	char* id;
	int size;
	struct schema* child;
	struct schema* brother;
} Schema;

typedef struct symtab
{
	char* name;
	int oid;
	ClassSymbol clazz;
	Schema schema;
	map_t locenv;
	int formals_size;
	struct symtab* formals[];
} Symbol;

int yysem();
int check_syntax_tree( Node* );
Symbol* create_symbol_table_element( Node*, int );

#endif

