#include "analyser.h"

#define STR_BUG "compiler bug"
#define STR_CONFLICT_TYPE "conflicting declaration (same variable defined twice in the same scope)"
#define STR_UNDECLARED "undeclared id"
#define STR_EMPTY_DECL "empty declaration"
#define STR_GENERAL "semantic error"
#define PRINT_ERROR(a,b) a ": " b

#define SEM_OK 0
#define SEM_ERROR -1

extern int yyparse();
extern Node* root;

stacklist scope;
Symbol* symbol_table;

int yysem()
{
	// Passing through the syntax tree to check semantic
	int result = yyparse();
	if( result == 0 )
	{
		symbol_table = check_function_subtree( root, 1 );
		return ( symbol_table == NULL ? SEM_ERROR : SEM_OK );
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
Symbol* check_function_subtree( Node* node, int oid_absolute )
{
	int oid_relative = 1;

	Symbol* element = create_symbol_table_element( node, &oid_absolute );

	// Updating the scope with the new function just created
	if( stacklist_push( &scope, (stacklist_t) element->locenv ) )
		return NULL;

	// Skipping the ID name of the function and look if there are parameters
	Node* current_node = node->child->brother;
	if( current_node->type == T_UNQUALIFIED_NONTERMINAL )
	{
		if( current_node->value.n_val != N_PAR_LIST )
			yysemerror( node, PRINT_ERROR( STR_BUG, "expected param list" ) );

		create_symbol_table_element( current_node, &oid_relative );
		associate_formals_parameters( current_node, element );
		current_node = current_node->brother;
	}
	current_node = current_node->brother;

	// Creating schemas for every section
	if( current_node->value.n_val == N_TYPE_SECT )
	{
		create_symbol_table_element( current_node, &oid_relative );
		current_node = current_node->brother;
	}
	if( current_node->value.n_val == N_VAR_SECT )
	{
		create_symbol_table_element( current_node, &oid_relative );
		current_node = current_node->brother;
	}
	if( current_node->value.n_val == N_CONST_SECT )
	{
		create_symbol_table_element( current_node, &oid_relative );
		current_node = current_node->brother;
	}
	if( current_node->value.n_val == N_FUNC_LIST )
	{
		Node* current_child = current_node->child;
		do
		{
			if( !insert_unconflicted_element( check_function_subtree( current_child, oid_absolute + 1 ) ) )
				yysemerror( current_child, STR_CONFLICT_TYPE );

			current_child = current_child->brother;
		} while( current_child != NULL );
	}

	// TODO: Processing body of the function
	//create_symbol_table_element( current_node, 0 );

	if( stacklist_pop( &scope ) )
		return NULL;

	return element;
}

/**
 * @brief Creates a symbol table for the node given
 * @todo Or this function adds to the scope or the caller does!
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
			analyse_decl_list( node->child, oid, CS_TYPE, FALSE );
			break;

		case N_VAR_SECT:
			fprintf( stdout, "Processing VAR_SECT\n" );
			analyse_decl_list( node->child, oid, CS_VAR, FALSE );
			break;

		case N_CONST_SECT:
			fprintf( stdout, "Processing CONST_SECT\n" );
			analyse_decl_list( node->child, oid, CS_CONST, TRUE );
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
			// FIXME The parameters should be added to the func as well
			fprintf( stdout, "Processing PAR_LIST\n" );
			analyse_decl_list( node->child, oid, CS_PAR, FALSE );
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
void analyse_decl_list( Node* node, int* oid, ClassSymbol clazz, Boolean hasAssignment )
{
	// Entering the decl list
	while( node != NULL )
	{
		Node* type_node = node->child;
		// TODO: fix for const_sect where the last brother is the value and not the type
		Node* domain_node;
		if( hasAssignment )
		{
			domain_node = type_node->brother;
			while( domain_node->brother->brother != NULL )
				domain_node = domain_node->brother;

			simplify_expression( domain_node->brother );
		}
		else
			domain_node = get_last_brother( type_node );

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
			if( !insert_unconflicted_element( aType ) )
				yysemerror( type_node, STR_CONFLICT_TYPE );

			type_node = type_node->brother;
		}

		node = node->brother;
	}
}

/**
 * @brief Adds all elements in the locenv as formal parameters of the given element.
 *
 * @param node PAR_LIST node
 * @param element Symbol table
 *
 * @return Returns the number of elements added as formal parameters.
 */
int associate_formals_parameters( Node* node, Symbol* element )
{
	// Cycling on the locenv of element knowing that its elements are only the formal paramenters
	Node* brothers = node->child;
	Node* current_node;
	Symbol* pointer;
	int actual_size = 4;
	element->formals_size = 0;
	element->formals = malloc( sizeof( Symbol* ) * actual_size );

	while( brothers != NULL )
	{
		current_node = brothers->child;

		while( current_node->brother != NULL )
		{
			// Assuming that I will find that element
			hashmap_get( element->locenv, current_node->value.s_val, (any_t*) &pointer );

			if( element->formals_size >= actual_size )
				element->formals = realloc( element->formals, sizeof( Symbol* ) * ( actual_size *= 2 ) );

			element->formals[ element->formals_size ] = pointer;

			element->formals_size++;
			current_node = current_node->brother;
		}

		brothers = brothers->brother;
	}
	
	return element->formals_size;
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
						// TODO Check for conflicts
						break;
					}

				case N_VECTOR_DOMAIN:
					result->type = TS_VECTOR;

					result->size = node->child->value.i_val;
					if( result->size <= 0 )
						yysemerror( node, PRINT_ERROR( STR_GENERAL, "vector size must be greater than zero" ) );
					result->child = create_schema( node->child->brother );
					break;

				default:
					yysemerror( node, PRINT_ERROR( STR_BUG, "create_schema instance" ) );
					break;
			}
			break;

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
 * @brief Simplifies as much as possible an expression, calculating every constant value
 * @todo
 *
 * @param node
 */
Boolean simplify_expression( Node* node )
{
	if( node == NULL )
		return TRUE;

	switch( node->type )
	{
		case T_INT_CONST:
		case T_CHAR_CONST:
		case T_REAL_CONST:
		case T_STR_CONST:
		case T_BOOL_CONST:
			return TRUE;

		case T_ID:
			return FALSE;

		default:
			break;
	}

	Node* left_child = node->child;
	Node* right_child = left_child->brother;
	if( simplify_expression( left_child ) && simplify_expression( right_child ) )
	{
		if( left_child->type != right_child->type )
			yysemerror( node, PRINT_ERROR( STR_GENERAL, "incompatible types" ) );

		switch( node->type )
		{
			case T_LOGIC_EXPR:
				node->type = T_BOOL_CONST;
				switch( node->value.q_val )
				{
					case Q_AND:
						node->value.b_val = left_child->value.b_val && right_child->value.b_val;
						break;

					case Q_OR:
						node->value.b_val = left_child->value.b_val || right_child->value.b_val;
						break;

					default:
						yysemerror( node, PRINT_ERROR( STR_BUG, "wrong syntax" ) );
				}
				break;

			case T_REL_EXPR:
				printf( "rel!\n" );
				// TODO
				return FALSE;

			case T_MATH_EXPR:
				node->type = left_child->type;
				if( node->type == T_INT_CONST )
					switch( node->value.q_val )
					{
						case Q_PLUS:
							node->value.i_val = left_child->value.i_val + right_child->value.i_val;
							break;

						case Q_MINUS:
							node->value.i_val = left_child->value.i_val - right_child->value.i_val;
							break;

						case Q_MULTIPLY:
							node->value.i_val = left_child->value.i_val * right_child->value.i_val;
							break;

						case Q_DIVIDE:
							node->value.i_val = left_child->value.i_val / right_child->value.i_val;
							break;

						default:
							yysemerror( node, PRINT_ERROR( STR_BUG, "wrong syntax" ) );
					}
				else
					switch( node->value.q_val )
					{
						case Q_PLUS:
							node->value.r_val = left_child->value.r_val + right_child->value.r_val;
							break;

						case Q_MINUS:
							node->value.r_val = left_child->value.r_val - right_child->value.r_val;
							break;

						case Q_MULTIPLY:
							node->value.r_val = left_child->value.r_val * right_child->value.r_val;
							break;

						case Q_DIVIDE:
							node->value.r_val = left_child->value.r_val / right_child->value.r_val;
							break;

						default:
							yysemerror( node, PRINT_ERROR( STR_BUG, "wrong syntax" ) );
					}

				break;

			case T_NEG_EXPR:
				node->type = T_BOOL_CONST;
				node->value.b_val = !left_child->value.b_val;
				break;

			case T_INSTANCE_EXPR:
				printf( "inst!\n" );
				// TODO
				return FALSE;

			case T_BUILT_IN_CALL:
				if( node->value.q_val == Q_TOREAL )
				{
					node->type = T_REAL_CONST;
					node->value.r_val = node->value.i_val;
				}
				else
				{
					node->type = T_INT_CONST;
					node->value.i_val = node->value.r_val;
				}

				break;

			default:
				yysemerror( node, PRINT_ERROR( STR_BUG, "simplify not an expression" ) );
		}

		/* free(left_child); */
		/* free(right_child); */
		node->child = NULL;

		return TRUE;
	}

	return FALSE;
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
		if( hashmap_get( current_scope->table, id, (any_t*) &result ) == MAP_OK )
			break;

		current_scope = current_scope->next;
	}

	return result;
}

/**
 * @brief Inserts an element checking name integrity in the rest of the scope.
 *
 * @param element
 *
 * @return 
 */
Boolean insert_unconflicted_element( Symbol* element )
{
	if( fetch_scope( element->name ) != NULL )
		return FALSE;

	hashmap_put( scope->table, element->name, element );
	return TRUE;
}

/**
 * @brief Error function occured on the given node knowing the error type.
 *
 * @param node
 * @param type
 *
 * @return 
 */
int yysemerror( Node* node, char* type )
{
	fprintf( stderr, "\n *** ERROR *** \n" );
	tree_print( node, 0 );

	if( node->type == T_ID )
		fprintf( stdout, "Line %d on symbol '%s': %s.\n", node->line, node->value.s_val, type );
	else
		fprintf( stdout, "Line %d: %s.\n", node->line, type );

	exit( 3 );
}
