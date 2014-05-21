#include "analyzer.h"

extern int yyerror(char*);
extern int yyparse();
extern Node* root;

char* STR_BUG = "compiler bug";

int yysem()
{
	// Passing through the syntax tree to check semantic
    int result = yyparse();
    if( result == 0 )
      return check_syntax_tree( root );
    else
      return result;
}

int check_syntax_tree( Node* node )
{
	int oid_relative = 1;
	int oid_absolute = 1;


	Symbol* element = create_symbol_table_element( node, oid_absolute );
	oid_absolute++;
	
	// Check if there is another func_decl and process it
	
	// Processing body of the function
}

Symbol* create_symbol_table_element( Node* node, int oid )
{
	Symbol* result = malloc( sizeof( Symbol ) );
	result->name = node->child->s_val;

	switch( node->type )
	{
		case N_TYPE_SECT:
			result->class = CLASS_TYPE;
			break;

		case N_VAR_SECT:
			result->class = CLASS_VAR;
			break;

		case N_CONST_DECL:
			result->class = CLASS_CONST;
			break;

		case N_FUNC_DECL:
			result->oid = oid;
			result->class = CLASS_FUNC;
			result->schema = look_into_schema_table( node->child->brother->brother );
			break;

		// PARAMETERS
	}

	
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
		return NULL; // TODO Fetch from stacklist

	Schema* result = malloc( sizeof( Schema ) );
	switch( node->type )
	{
		case T_ATOMIC_DOMAIN:
			break;
			
		case T_INSTANCE_EXPR:
			switch( node->value.q_val )
			{
				case Q_STRUCT:
				{
					Schema** current_schema = &(result->child);
					Node* current_node = node->child;
					if( current_node == NULL )
						yyerror( "empty struct" );

					do
					{
						*current_schema = create_schema_attribute( current_node->child );
						current_node = current_node->brother;
						current_schema = &( (*current_schema)->brother );
					} while( current_node != NULL );
					break;
				}

				case Q_VECTOR:
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
