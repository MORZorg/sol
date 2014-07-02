#ifndef __ANALYSER_H__
#define __ANALYSER_H__

#include "def.h"
#include "../libs/hashmap.h"
#include "../libs/stacklist.h"

#define SEM_OK 0
#define SEM_ERROR -1

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
	int nesting;
    int last_oid;
	int formals_size;
	struct symtab** formals;
} Symbol;

int yysem();
Symbol* check_function_subtree( Node*, int* );
Symbol* create_symbol_table_element( Node*, int* );
void analyse_decl_list( Node*, int*, ClassSymbol, Boolean hasAssignment );
int associate_formals_parameters( Node*, Symbol* );
Schema* create_schema( Node* );
Schema* create_schema_attribute( Node* );
Boolean simplify_expression( Node* );
Schema* infere_expression_schema( Node* );
Schema* infere_lhs_schema( Node*, Boolean );
Boolean schema_check( Schema*, Schema* );
Boolean type_check( Node* );
Symbol* fetch_scope( char* );
Boolean insert_unconflicted_element( Symbol* );

int yysemerror( Node*, char* );

#endif // __ANALYZER_H__

