#include "analyzer.h"
#include "stacklist.h"

#define STR_BUG "compiler bug"
#define STR_UNDECLARED "undeclared id"
#define STR_EMPTY_DECL "empty declaration"

extern int yyerror(char*);
extern int yyparse();
extern Node* root;

stacklist scope;

int yysem()
{
	// Passing through the syntax tree to check semantic
    int result = yyparse();
    if( result == 0 )
      return check_function_subtree( root );
    else
      return result;
}

int check_function_subtree( Node* node )
{
	int oid_relative = 1;
	int oid_absolute = 1;
	stacklist scope = new_stack();

	Symbol* element = create_symbol_table_element( node, oid_absolute );
	oid_absolute++;

	if( stacklist_push( scope, (any_t) element ) )
		return STACK_ERROR;
	
	// Check if there is another func_decl and process it
	
	// Processing body of the function
}

Symbol* create_symbol_table_element( Node* node, int oid )
{
	Symbol* result = malloc( sizeof( Symbol ) );
	result->name = node->child->s_val;
	result->oid = oid;

	switch( node->type )
	{
		case N_TYPE_SECT:
			result->class = CS_TYPE;
			break;

		case N_VAR_SECT:
			result->class = CS_VAR;
			break;

		case N_CONST_DECL:
			result->class = CS_CONST;
			break;

		case N_FUNC_DECL:
			result->class = CS_FUNC;
			result->schema = look_into_schema_table( node->child->brother->brother );
			break;

		// PARAMETERS
	}

	result->next = NULL;

	return result;
}

/**
 * @brief Transmutes a syntax domain node into a schema node.
 * Expects a syntax node regarding a DOMAIN definition. In other cases
 * `yyerror` will be called.
 *
 * @param node
 *
 * @return 
 */
Schema* create_schema( Node* node )
{
	if( node->type == T_ID_DOMAIN )
	{
		Symbol* definition = fetch_scope( node->value.s_val );
		if( definition == NULL )
			yyerror( STR_UNDECLARED );
		else
			return definition->schema;
	}

	Schema* result = malloc( sizeof( Schema ) );
	switch( node->type )
	{
		case T_ATOMIC_DOMAIN:
			switch( node->value.q_val )
			{
				case Q_CHAR:
					result->type = TS_CHAR;
					break;

				case Q_INT:
					result->type = TS_INT;
					break;

				case Q_REAL:
					result->type = TS_REAL;
					break;

				case Q_STRING:
					result->type = TS_STRING;
					break;

				case Q_BOOL:
					result->type = TS_BOOL;
					break;

				default:
					yyerror( STR_BUG );
					break;
			}
			break;
			
		case T_INSTANCE_EXPR:
			switch( node->value.q_val )
			{
				case Q_STRUCT:
				{
					result->type = TS_STRUCT;

					Schema** current_schema = &(result->child);
					Node* current_node = node->child;
					if( current_node == NULL )
						yyerror( STR_EMPTY_DECL );

					do
					{
						*current_schema = create_schema_attribute( current_node->child );
						current_node = current_node->brother;
						current_schema = &( (*current_schema)->brother );
					} while( current_node != NULL );
					break;
				}

				case Q_VECTOR:
					result->type = TS_VECTOR;

					result->size = node->child->value.i_val;
					result->child = create_schema( node->child->brother );
					break;

				default:
					yyerror( STR_BUG );
					break;
			}

		default:
			yyerror( STR_BUG );
			break;
	}
	return result;
}

/**
 * @brief Transmutes a syntax declaration node into a schema attribute node.
 * Expects a syntax node regarding an ID child of a DECL definition. In other
 * cases `yyerror` will bel called.
 *
 * @param node
 *
 * @return 
 */
Schema* create_schema_attribute( Node* node )
{
	Schema* result = malloc( sizeof( Schema ) );
	result->type = TS_ATTR;
	result->id = node->value.s_val;

	if( node->brother == NULL )
		yyerror( STR_BUG );

	switch( node->brother->type )
	{
		case T_ID:
			result->brother = create_schema_attribute( node->brother );
			result->child = result->brother->child;
			break;

		case T_ID_DOMAIN:
		case T_ATOMIC_DOMAIN:
		case T_INSTANCE_EXPR:
			result->child = create_schema( node->brother );
			break;

		default:
			yyerror( STR_BUG );
	}

	return result;
}

/**
 * @brief Searches for an ID in the actual scope and, eventually, its fathers.
 *
 * @param id
 *
 * @return 
 */
Symbol* fetch_scope( char* id )
{
	Symbol* result = NULL;

	stacklist current_scope = scope;
	while( current_scope != NULL )
	{
		if( hashmap_get(current_scope->table, id, (any_t*) &result ) == STACK_OK )
			break;

		current_scope = current_scope->next;
	}

	return result;
}

