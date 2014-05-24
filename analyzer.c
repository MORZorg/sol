#include "analyzer.h"

#define STR_BUG "compiler bug"
#define STR_UNDECLARED "undeclared id"
#define STR_EMPTY_DECL "empty declaration"
#define PRINT_ERROR(a,b) a ": " b

#define SEM_OK 0
#define SEM_ERROR -1

extern int yyparse();
extern Node* root;

stacklist scope;

int yysem()
{
	// Passing through the syntax tree to check semantic
	int result = yyparse();
	if( result == 0 )
	{
		scope = new_stack();
		return check_function_subtree( root, 1 );
	}
	else
		return result;
}

/**
 * @brief Creates the whole symbol table, knowing that every node precessed will be a FUNC_STAT.
 *
 * @param node
 *
 * @return 
 */
int check_function_subtree( Node* node, int oid_absolute )
{
	int oid_relative = 1;

	Symbol* element = create_symbol_table_element( node, &oid_absolute );

    // Updating the scope with the new function just created
	if( stacklist_push( &scope, (any_t) element->locenv ) )
		return STACK_ERROR;

    // Skipping the ID name of the function and look if there are parameters
    Node* current_node = node->child->brother;
    if( current_node->type == T_UNQUALIFIED_NONTERMINAL ) // N_PARAM_LIST value
    {
        create_symbol_table_element( current_node, &oid_relative );
        current_node = current_node->brother;
    }

    // Creating schemas for every section
    create_symbol_table_element( ( current_node = current_node->brother ), &oid_relative ); // TYPE_SECT
    create_symbol_table_element( ( current_node = current_node->brother ), &oid_relative ); // VAR_SECT
    create_symbol_table_element( ( current_node = current_node->brother ), &oid_relative ); // CONST_SECT 

    // Checking for other functions declarations
    if( ( current_node = current_node->brother )->value.n_val == N_FUNC_DECL )
    {
        check_function_subtree( current_node, oid_absolute + 1 );
        current_node = current_node->brother;
    }

	// TODO: Processing body of the function
	//create_symbol_table_element( current_node, 0 );

    return SEM_OK;
}

/**
 * @brief Creates a symbol table for the node given
 *
 * @param node
 * @param oid
 *
 * @return 
 */
Symbol* create_symbol_table_element( Node* node, int* oid )
{
	Symbol* result = malloc( sizeof( Symbol ) );
	result->oid = (*oid);

	switch( node->value.n_val )
	{
		case N_TYPE_SECT:
			fprintf( stdout, "Processing TYPE_SECT\n" );
			analyze_decl_list( node->child, oid, CS_TYPE );
			break;

		case N_VAR_SECT:
			fprintf( stdout, "Processing VAR_SECT\n" );
			analyze_decl_list( node->child, oid, CS_VAR );
			break;

		case N_CONST_SECT:
			fprintf( stdout, "Processing CONST_SECT\n" );
			analyze_decl_list( node->child, oid, CS_CONST );
			break;

		case N_FUNC_DECL:
			fprintf( stdout, "Processing FUNC_SECT\n" );
			result->name = node->child->value.s_val;
			result->clazz = CS_FUNC;
			// Checking if the function has or not a parameter list
			if( node->child->brother->type == T_UNQUALIFIED_NONTERMINAL )
				result->schema = create_schema( node->child->brother->brother );
			else
				result->schema = create_schema( node->child->brother );

			result->locenv = hashmap_new();
			break;

		case N_PAR_LIST:
			fprintf( stdout, "Processing PAR_SECT\n" );
			analyze_decl_list( node->child, oid, CS_PAR );
			break;

		case N_FUNC_BODY:
			fprintf( stdout, "Loading Body..." );
			break;

		default:
			yysemerror( node, PRINT_ERROR( STR_BUG, "create_symbol_table_element" ) );
	}

	return result;
}

/**
 * @brief 
 *
 * @param node We expect this to be the first element of the decl list.
 * @param oid
 * @param clazz
 */
void analyze_decl_list( Node* node, int* oid, ClassSymbol clazz  )
{
	// Entering the decl list
	while( node != NULL )
	{
	  Node* type_node = node->child;
	  // TODO: fix for const_sect where the last brother is the value and not the type
	  Node* domain_node = get_last_brother( type_node );
	  Schema* domain_schema = create_schema( domain_node );
	  while( type_node != domain_node )
	  {
		  Symbol* aType = malloc( sizeof( Symbol ) );
		  aType->name = type_node->value.s_val;
		  aType->oid = (*oid);
		  (*oid)++;
		  aType->clazz = clazz;
		  aType->schema = domain_schema; 

		  // Adding the new type to the locenv in the scope in which is defined
		  hashmap_put( scope->table, aType->name, aType );

		  type_node = type_node->brother;
	  }

	  node = node->brother;
	}
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
			yysemerror( node, PRINT_ERROR( STR_UNDECLARED, "null definition" ) );
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
					yysemerror( node, PRINT_ERROR( STR_BUG, "create_schema atomic" ) );
					break;
			}
			break;

		case T_UNQUALIFIED_NONTERMINAL:
			switch( node->value.n_val )
			{
				case N_STRUCT_DOMAIN:
				{
					result->type = TS_STRUCT;

					Schema** current_schema = &(result->child);
					Node* current_node = node->child;
					if( current_node == NULL )
						yysemerror( node, STR_EMPTY_DECL );

					do
					{
						*current_schema = create_schema_attribute( current_node->child );
						current_node = current_node->brother;
						current_schema = &( (*current_schema)->brother );
					} while( current_node != NULL );
					break;
				}

				case N_VECTOR_DOMAIN:
					result->type = TS_VECTOR;

					result->size = node->child->value.i_val;
					result->child = create_schema( node->child->brother );
					break;

				default:
					yysemerror( node, PRINT_ERROR( STR_BUG, "create_schema instance" ) );
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
						yysemerror( node, STR_EMPTY_DECL );

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
					yysemerror( node, PRINT_ERROR( STR_BUG, "create_schema instance" ) );
					break;
			}
			break;


//		case T_INT_CONST:
//		case T_CHAR_CONST:
//		case T_REAL_CONST:
//		case T_STR_CONST:
//		case T_BOOL_CONST:
//			fprintf( stderr, "I should not be here, but..\n" );
//			break;

		default:
			yysemerror( node, PRINT_ERROR( STR_BUG, "create_schema general" ) );
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
		yysemerror( node, PRINT_ERROR( STR_BUG, "create_schema_attribute brother" ) );

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
			yysemerror( node, PRINT_ERROR( STR_BUG, "create_schema_attribute" ) );
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

int yysemerror( Node* node, char* type )
{
	fprintf( stderr, "\n *** ERROR *** \n" );
	fprintf( stderr, "Type: %d\t", node->type );
	fprintf( stderr, "(N)Value: %d\n", node->value.n_val );
	tree_print( node, 0 );
	fprintf( stdout, "Line %d: %s.\n", node->line, type );
	exit( 3 );
}
