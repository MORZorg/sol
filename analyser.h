#ifndef __ANALYSER_H__
#define __ANALYSER_H__

#include "def.h"
#include "hashmap.h"
#include "stacklist.h"

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
	int formals_size;
	struct symtab** formals;
} Symbol;

// TODO Resort (at least to be coherent with the .c file)
int yysem();
Symbol* check_function_subtree( Node*, int );
Symbol* create_symbol_table_element( Node*, int* );
Schema* create_schema( Node* );
Schema* create_schema_attribute( Node* );
Symbol* fetch_scope( char* );
void analyse_decl_list( Node*, int*, ClassSymbol, Boolean hasAssignment );
Boolean simplify_expression( Node* );
Boolean insert_unconflicted_element( Symbol* );
int associate_formals_parameters( Node*, Symbol* );
Schema* infere_expression_schema( Node* );
Boolean schema_check( Schema*, Schema* );
Schema* type_check( Node* );

int yysemerror( Node*, char* );

// FIXME Used only for debug and put in the end due to cyclicity.
#include "tree_print.h"

#endif // __ANALYZER_H__

