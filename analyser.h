#ifndef __ANALYSER_H__
#define __ANALYSER_H__

#include "def.h"
#include "hashmap.h"
#include "stacklist.h"

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
	Schema* schema;
	map_t locenv;
	int formals_size;
	struct symtab* formals[];
} Symbol;

int yysem();
int check_function_subtree( Node*, int );
Symbol* create_symbol_table_element( Node*, int* );
Schema* create_schema( Node* );
Schema* create_schema_attribute( Node* );
Symbol* fetch_scope( char* );
void analyse_decl_list( Node*, int*, ClassSymbol, Boolean hasAssignment );
Boolean simplify_expression( Node* );

int yysemerror( Node*, char* );

// FIXME Used only for debug and put in the end due to cyclicity.
#include "tree_print.h"

#endif // __ANALYZER_H__

