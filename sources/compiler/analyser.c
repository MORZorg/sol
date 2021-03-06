#include "analyser.h"
#include "tree_print.h"

#define STR_BUG "compiler bug"

#define STR_CONFLICT_SCOPE "conflicting declaration (same variable defined twice in the same scope)"
#define STR_CONFLICT_STRUCT "conflicting declaration (same field defined twice in the same struct)"
#define STR_CONFLICT_TYPE "conflicting types"
#define STR_WRONG_CLASS "wrong class"

#define STR_SCHEMA_DIFFERENT "incompatible types"

#define STR_VECTOR_SIZE "vectors must be of the same size"
#define STR_VECTOR_TYPE "vector elements must be of the same type"

#define STR_STRUCT_NAMES "record fields must have the same name"
#define STR_STRUCT_TYPES "record fields must have the same type"

#define STR_NO_RETURN "no RETURN indicated or not all paths could end"
#define STR_CODE_AFTER_RETURN "there is more code after the end of the program"
#define STR_ASSIGN_TYPE "type mismatch while assigning" // "this could not be a lhs term"
#define STR_INDEXING "this statement could not be indexed"
#define STR_FIELDING "this statement could not be accessed as a record"

#define STR_COND_EXPR "the condition must be boolean"

#define STR_RETURN_TYPE "the return type mismatches the type of the function"

#define STR_TOO_FEW_PARAM "not enough parameters for that function call"
#define STR_TOO_MANY_PARAM "too many parameters for that function call"
#define STR_PARAM_TYPE "mismatch type between actual and formal parameters"

#define STR_UNDECLARED "undeclared id"
#define STR_EMPTY_DECL "empty declaration"
#define STR_GENERAL "semantic error"

#define PRINT_ERROR(a,b) a ": " b

extern int yyparse();
extern Node* root;

stacklist scope;
Symbol* symbol_table;

/**
 * @brief Function which starts the semantic analysis of the given syntax tree.
 *
 * @return 
 */
int yysem()
{
	// Passing through the syntax tree to check semantic
	int result = yyparse();

	if( result == 0 )
	{
		int oid = 0;
		symbol_table = check_function_subtree( root, &oid );
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
Symbol* check_function_subtree( Node* node, int* oid_absolute )
{
	(*oid_absolute)++;
	Symbol* element = create_symbol_table_element( node, oid_absolute );
    element->last_oid = 1;

	if( element->nesting != 0 )
		element->nesting = ( (Symbol*) scope->function )->nesting + 1;

	// Updating the scope with the new function just created
	if( stacklist_push( &scope, (stacklist_t) element ) )
		return NULL;

	// Skipping the ID name of the function and look if there are parameters
	Node* current_node = node->child->brother;
	if( current_node->type == T_UNQUALIFIED_NONTERMINAL )
	{
		if( current_node->value.n_val != N_PAR_LIST )
			yysemerror( node, PRINT_ERROR( STR_BUG, "expected param list" ) );

		create_symbol_table_element( current_node, &element->last_oid );
		associate_formals_parameters( current_node, element );
		current_node = current_node->brother;
	}
	current_node = current_node->brother;

	// Creating schemas for every section
	if( current_node->value.n_val == N_TYPE_SECT )
	{
		create_symbol_table_element( current_node, &element->last_oid );
		current_node = current_node->brother;
	}
	if( current_node->value.n_val == N_VAR_SECT )
	{
		create_symbol_table_element( current_node, &element->last_oid );
		current_node = current_node->brother;
	}
	if( current_node->value.n_val == N_CONST_SECT )
	{
		create_symbol_table_element( current_node, &element->last_oid );
		current_node = current_node->brother;
	}
	if( current_node->value.n_val == N_FUNC_LIST )
	{
		Node* current_child = current_node->child;
		do
		{
			if( !insert_unconflicted_element( check_function_subtree( current_child, oid_absolute ) ) )
				yysemerror( current_child, STR_CONFLICT_SCOPE );

			current_child = current_child->brother;
		} while( current_child != NULL );

		current_node = current_node->brother;
	}

	// Check that the function name matches the body and end name
	if( element->name != current_node->child->value.s_val )
		yysemerror( current_node->child, PRINT_ERROR( STR_GENERAL, "the function body name must match the function's name" ) );

	type_check( current_node );

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
			analyse_decl_list( node->child, oid, CS_TYPE, FALSE );
			break;

		case N_VAR_SECT:
			analyse_decl_list( node->child, oid, CS_VAR, FALSE );
			break;

		case N_CONST_SECT:
			analyse_decl_list( node->child, oid, CS_CONST, TRUE );
			break;

		case N_FUNC_DECL:
			result->name = node->child->value.s_val;
			result->clazz = CS_FUNC;
			// Checking if the function has or not a parameter list
			if( node->child->brother->type == T_UNQUALIFIED_NONTERMINAL )
				result->schema = create_schema( node->child->brother->brother );
			else
				result->schema = create_schema( node->child->brother );

			result->locenv = hashmap_new();
			if( (*oid) == 1 )
				result->nesting = 0;
			else
				result->nesting = -1;
			break;

		case N_PAR_LIST:
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
void analyse_decl_list( Node* node, int* oid, ClassSymbol clazz, Boolean has_assignment )
{
	// Entering the decl list
	while( node != NULL )
	{
		Node* type_node = node->child;
		Node* domain_node;
		if( has_assignment )
		{
			domain_node = type_node->brother;
			while( domain_node->brother->brother != NULL )
				domain_node = domain_node->brother;

			simplify_expression( domain_node->brother );
		}
		else
			domain_node = get_last_brother( type_node );

		Schema* domain_schema = create_schema( domain_node );
		simplify_expression( domain_node->brother );
		if( has_assignment && !schema_check( domain_schema, infere_expression_schema( domain_node->brother ) ) )
			yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "decl_list" ) );
		
		while( type_node != domain_node )
		{
			Symbol* a_type = malloc( sizeof( Symbol ) );
			a_type->name = type_node->value.s_val;
            if( clazz != CS_TYPE )
            {
              a_type->oid = (*oid);
              (*oid)++;
            }
			a_type->clazz = clazz;
			a_type->schema = domain_schema; 
			a_type->nesting = ( (Symbol*) scope->function )->nesting;

			// Adding the new type to the locenv in the scope in which is defined
			if( !insert_unconflicted_element( a_type ) )
				yysemerror( type_node, STR_CONFLICT_SCOPE );

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
	// Cycling on the locenv of element knowing that its elements are only the formal parameters
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
						
						// Check for conflicts with a `wonderful' O(n log(n))
						Schema* reference_attr = result->child;
						while( reference_attr != NULL )
						{
							Schema* checked_attr = reference_attr->brother;

							while( checked_attr != NULL )
							{
								if( reference_attr->id == checked_attr->id )
									yysemerror( node, STR_CONFLICT_STRUCT );

								checked_attr = checked_attr->brother;
							}

							reference_attr = reference_attr->brother;
						}	
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

		// Check for other types of domains, like ID, ATOMIC, STRUCT AND VECTOR (UNQUALIFIED) and an INSTANCE
		case T_ID_DOMAIN:
		case T_ATOMIC_DOMAIN:
		case T_INSTANCE_EXPR:
		case T_UNQUALIFIED_NONTERMINAL:
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

        case T_ATOMIC_DOMAIN:
		case T_ID_DOMAIN:
		case T_ID:
			return FALSE;


		case T_INSTANCE_EXPR:
		{
			Boolean result = TRUE;
			
			Node* current_node = node->child;
			while( current_node != NULL ) 
			{
				result &= simplify_expression( current_node );
				current_node = current_node->brother;
			}

			return result;
		}

		default:
			break;
	}

	Node* left_child = node->child;
	Node* right_child = left_child->brother;
	if( simplify_expression( left_child ) && simplify_expression( right_child ) )
	{
		// NOTE: This check is disabled since it happens very rarely and it's
		// quite complex to implement. It should be possible, though.
		if( node->value.q_val == Q_IN )
			return FALSE;

		if( right_child != NULL && left_child->type != right_child->type )
			yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "simplify" ) );

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
				node->type = T_BOOL_CONST;
				switch( node->value.q_val )
				{
					case Q_EQ:
						node->value.b_val = ( left_child->value.i_val == right_child->value.i_val );
						break;

					case Q_NEQ:
						node->value.b_val = ( left_child->value.i_val != right_child->value.i_val );
						break;

					case Q_GT:
						node->value.b_val = ( left_child->value.i_val > right_child->value.i_val );
						break;

					case Q_GEQ:
						node->value.b_val = ( left_child->value.i_val >= right_child->value.i_val );
						break;

					case Q_LT:
						node->value.b_val = ( left_child->value.i_val < right_child->value.i_val );
						break;

					case Q_LEQ:
						node->value.b_val = ( left_child->value.i_val <= right_child->value.i_val );
						break;

					case Q_IN:
					{
						Node* eq = new_qualified_node( T_REL_EXPR, Q_EQ );
						eq->child = left_child;
						Node* current_right_children = right_child->child;
						node->value.b_val = FALSE;
						while( current_right_children != NULL )
						{
							eq->child->brother = current_right_children;
							node->value.b_val |= simplify_expression( eq );
							current_right_children = current_right_children->brother;
						}

						left_child->brother = right_child;
						break;
					}
						
					default:
						yysemerror( node, PRINT_ERROR( STR_BUG, "wrong syntax" ) );
						break;
				}

#ifdef DEBUG
				fprintf( stderr, "Result: %d\n", node->value.b_val );
#endif
				break;

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
				node->type = left_child->type;
				switch( left_child->type )
				{
					case T_BOOL_CONST:
						node->value.b_val = !left_child->value.b_val;
						break;

					case T_INT_CONST:
						node->value.i_val = -left_child->value.i_val;
						break;

					case T_REAL_CONST:
						node->value.r_val = -left_child->value.r_val;
						break;

					default:
						yysemerror( node, PRINT_ERROR( STR_BUG, "wrong syntax" ) );
				}
				break;

			case T_INSTANCE_EXPR:
			{
				Node* current_child = node->child;
				node->value.b_val = TRUE;
				while( current_child != NULL )
				{
					node->value.b_val &= simplify_expression( current_child );
					current_child = current_child->brother;
				}
				break;
			}

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

		free(left_child);
		free(right_child);
		node->child = NULL;

		return TRUE;
	}

	return FALSE;
}

/**
 * @brief Function that infers the schema of an expr, given the corresponding syntax node.
 *
 * @param node
 *
 * @return Returns the schema of the expr otherwise an error will be thrown.
 */
Schema* infere_expression_schema( Node* node )
{
	Schema* result = NULL;

	switch( node->type )
	{
		case T_LOGIC_EXPR:
		case T_REL_EXPR:
		{
			Schema* left_argument = infere_expression_schema( node->child );
			Schema* right_argument = infere_expression_schema( node->child->brother );
			if( node->value.q_val != Q_IN && !schema_check( left_argument, right_argument ) )
				yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "infere" ) );

			switch( node->value.q_val )
			{
				/* Logic expr */
				case Q_AND:
				case Q_OR:
					if( left_argument->type != TS_BOOL )
						yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "logic" ) );
					break;

				/* Rel expr */
				case Q_EQ:
				case Q_NEQ:
					break;

				case Q_GT:
				case Q_GEQ:
				case Q_LT:
				case Q_LEQ:
					switch( left_argument->type )
					{
						case TS_CHAR:
						case TS_INT:
						case TS_REAL:
						case TS_STRING:
							break;

						default:
							yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "GT" ) );
					}
					break;

				case Q_IN:
					if( right_argument->type != TS_VECTOR
						|| !schema_check( left_argument, right_argument->child ) )
						yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "IN" ) );
					break;

				default:
					yysemerror( node, PRINT_ERROR( STR_BUG, "infere rel/logic expression" ) );
			}
			result = malloc( sizeof( Schema ) );
			result->type = TS_BOOL;
			break;
		}

		case T_MATH_EXPR:
			result = infere_expression_schema( node->child );
			if( !schema_check( result, infere_expression_schema( node->child->brother ) ) )
				yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "math" ) );
			if( result->type != TS_INT && result->type != TS_REAL )
				yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "math 2" ) );
			break;

		case T_NEG_EXPR:
			result = infere_expression_schema( node->child );
			switch( node->value.q_val )
			{
				case Q_MINUS:
					if( result->type != TS_INT && result->type != TS_REAL )
						yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "neg" ) );
					break;

				case Q_NOT:
					if( result->type != TS_BOOL )
						yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "not" ) );
					break;

				default:
					yysemerror( node, PRINT_ERROR( STR_BUG, "infere neg expression" ) );
			}
			break;

		case T_INSTANCE_EXPR:
			result = malloc( sizeof( Schema ) );
			switch( node->value.q_val )
			{
				case Q_STRUCT:
				{
					result->type = TS_STRUCT;
					Node* current_child = node->child;
					Schema** current_attribute = &(result->child);
					while( current_child != NULL )
					{
						*current_attribute = malloc( sizeof( Schema ) );
						(*current_attribute)->type = TS_ATTR;
						(*current_attribute)->child = infere_expression_schema( current_child );

						current_child = current_child->brother;
						current_attribute = &(*current_attribute)->brother;
					}
					break;
				}

				case Q_VECTOR:
				{
					result->type = TS_VECTOR;
					result->size = 1;
					result->child = infere_expression_schema( node->child );

					Node* current_child = node->child->brother;
					while( current_child != NULL )
					{
						if( !schema_check( result->child, infere_expression_schema( current_child ) ) )
							yysemerror( current_child, PRINT_ERROR( STR_CONFLICT_TYPE, "vector" ) );

						result->size++;
						current_child = current_child->brother;
					}
					break;
				}

				default:
					yysemerror( node, PRINT_ERROR( STR_BUG, "wrong qualifier" ) );
			}
			break;

		case T_BUILT_IN_CALL:
			result = malloc( sizeof( Schema ) );
			switch( node->value.q_val )
			{
				case Q_TOINT:
					if( infere_expression_schema( node->child )->type != TS_REAL )
						yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "toint" ) );

					result->type = TS_INT;
					break;

				case Q_TOREAL:
					if( infere_expression_schema( node->child )->type != TS_INT )
						yysemerror( node, PRINT_ERROR( STR_CONFLICT_TYPE, "toreal" ) );

					result->type = TS_REAL;
					break;

				default:
					yysemerror( node, PRINT_ERROR( STR_BUG, "wrong qualifier" ) );
			}
			break;

		case T_INT_CONST:
			result = malloc( sizeof( Schema ) );
			result->type = TS_INT;
			break;

		case T_CHAR_CONST:
			result = malloc( sizeof( Schema ) );
			result->type = TS_CHAR;
			break;

		case T_REAL_CONST:
			result = malloc( sizeof( Schema ) );
			result->type = TS_REAL;
			break;

		case T_STR_CONST:
			result = malloc( sizeof( Schema ) );
			result->type = TS_STRING;
			break;

		case T_BOOL_CONST:
			result = malloc( sizeof( Schema ) );
			result->type = TS_BOOL;
			break;

		case T_ID:
            result = infere_lhs_schema( node, FALSE );
            break;

		case T_UNQUALIFIED_NONTERMINAL:
			result = malloc( sizeof( Schema ) );
			switch( node->value.n_val )
			{
				case N_DYNAMIC_INPUT:
					if( node->child->brother != NULL )
					{
						if( infere_expression_schema( node->child )->type != TS_STRING )
							yysemerror( node->child, PRINT_ERROR( STR_CONFLICT_TYPE, "dynamic input" ) );

						result = create_schema( node->child->brother );
					}
					else
						result = create_schema( node->child );
					break;

				case N_DYNAMIC_OUTPUT:
					if( node->child->brother != NULL )
					{
						if( infere_expression_schema( node->child )->type != TS_STRING )
							yysemerror( node->child, PRINT_ERROR( STR_CONFLICT_TYPE, "dynamic output" ) );

						result = infere_expression_schema( node->child->brother );
					}
					else
						result = infere_expression_schema( node->child );
					break;

				case N_FIELDING:
				case N_INDEXING:
					result = infere_lhs_schema( node, FALSE );
					break;

				case N_FUNC_CALL:
				{
					Symbol* function = fetch_scope( node->child->value.s_val );
					if( function == NULL )
						yysemerror( node->child, PRINT_ERROR( STR_UNDECLARED, "function call" ) );
					result = function->schema;

					int i;
					Node* current_node = node->child->brother;
					for( i = 0; i < function->formals_size; i++ )
					{
						if( current_node == NULL )
							yysemerror( node, STR_TOO_FEW_PARAM );

						if( !schema_check( function->formals[ i ]->schema, infere_expression_schema( current_node ) ) )
							yysemerror( current_node, STR_PARAM_TYPE );

						current_node = current_node->brother;
					}

					if( current_node != NULL )
						yysemerror( node, STR_TOO_MANY_PARAM );
					break;
				}

				case N_COND_EXPR:
				{
					Node* current_node = node->child;
					if( infere_expression_schema( current_node )->type != TS_BOOL )
						yysemerror( current_node, STR_COND_EXPR );

					result = infere_expression_schema( current_node = current_node->brother );
					for( current_node = current_node->brother; current_node != NULL; current_node = current_node->brother )
						if( !schema_check( result, infere_expression_schema( current_node ) ) )
							yysemerror( current_node, PRINT_ERROR( STR_CONFLICT_TYPE, "cond expr" ) );

					break;
				}

				case N_ELSIF_EXPR:
					if( infere_expression_schema( node->child )->type != TS_BOOL )
						yysemerror( node->child, STR_COND_EXPR );

					result = infere_expression_schema( node->child->brother );
					break;

				default:
					yysemerror( node, PRINT_ERROR( STR_BUG, "infere unknown T_UNQUALIFIED_NONTERMINAL" ) );
			}
			break;

		default:
			yysemerror( node, PRINT_ERROR( STR_BUG, "infere unknown type" ) );
			break;
	}

	return result;
}

/**
 * @brief Function that infers the schema of a lhs node.
 *
 * @param node The node of the syntax tree.
 * @param is_assigned Boolean that indicates if the lhs is an assignment (TRUE) or not.
 *
 * @return Returns the inferred schema of the node.
 */
Schema* infere_lhs_schema( Node* node, Boolean is_assigned )
{
	if( node->type == T_ID )
	{
		Symbol* variable = fetch_scope( node->value.s_val );
		if( !variable )
			yysemerror( node, PRINT_ERROR( STR_UNDECLARED, "LHS" ) );

		switch( variable->clazz )
		{
			case CS_TYPE:
			case CS_FUNC:
				yysemerror( node, PRINT_ERROR( STR_WRONG_CLASS, "not a variable" ) );
				break;

			case CS_VAR:
			case CS_PAR:
				return variable->schema;

			case CS_CONST:
				if( is_assigned )
					yysemerror( node, PRINT_ERROR( STR_WRONG_CLASS, "can't assign a constant" ) );
				else
					return variable->schema;
		}
	}

	Schema* result = malloc( sizeof( Schema ) );

	switch( node->value.n_val )
	{
		case N_FIELDING:
			result = infere_lhs_schema( node->child, is_assigned );
			if( result->type != TS_STRUCT )
				yysemerror( node->child, PRINT_ERROR( STR_CONFLICT_TYPE, "not a struct" ) );

			result = result->child;
			while( result != NULL )
			{
				if( result->id == node->child->brother->value.s_val )
					return result->child;

				result = result->brother;
			}

			yysemerror( node->child->brother, PRINT_ERROR( STR_UNDECLARED, "not a struct attribute" ) );
			break;

		case N_INDEXING:
			result = infere_lhs_schema( node->child, is_assigned );
			if( result->type != TS_VECTOR )
				yysemerror( node->child, PRINT_ERROR( STR_CONFLICT_TYPE, "not a vector" ) );

			simplify_expression( node->child->brother );
			if( infere_expression_schema( node->child->brother )->type != TS_INT )
				yysemerror( node->child, PRINT_ERROR( STR_CONFLICT_TYPE, "expression must be integer" ) );
			
			result = result->child;
			break;

		default:
			yysemerror( node, PRINT_ERROR( STR_BUG, "unknown unqualified nonterminal expression" ) );
			break;
	}

	return result;
}

/**
 * @brief Compares the two given schemas.
 *
 * @param first
 * @param second
 *
 * @return Returns TRUE if the two schemas are the same.
 */
Boolean schema_check( Schema* first, Schema* second )
{
	if( first->type != second->type )
		return FALSE;

	switch( first->type )
	{
		case TS_VECTOR:
			if( first->size != second->size )
				return FALSE;
			if( !schema_check( first->child, second->child ) )
				return FALSE;

			break;

		case TS_STRUCT:
			{
				Schema* first_child = first->child;
				Schema* second_child = second->child;
				while( first_child != NULL && second_child != NULL )
				{
					schema_check( first_child, second_child );
					first_child = first_child->brother;
					second_child = second_child->brother;
				}
				break;
			}

		case TS_ATTR:
			if( !schema_check( first->child, second->child ) )
				return FALSE;
			break;

		case TS_CHAR:
		case TS_INT:
		case TS_REAL:
		case TS_STRING:
		case TS_BOOL:
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

/**
 * @brief Function that proceeds doing type checking on the whole syntax tree, also including function bodies.
 *
 * @param node
 *
 * @return Returns TRUE if there are not type errors.
 */
Boolean type_check( Node* node )
{
	Boolean has_return = FALSE;

	switch( node->value.n_val )
	{
		case N_FUNC_DECL:
		case N_PAR_LIST:
		case N_DECL:
		case N_STRUCT_DOMAIN:
		case N_VECTOR_DOMAIN:
		case N_TYPE_SECT:
		case N_VAR_SECT:
		case N_CONST_SECT:
		case N_CONST_DECL:
		case N_FUNC_LIST:
		case N_FIELDING:
		case N_INDEXING:
			// Just for switch completeness
			break;

		case N_FUNC_BODY:
		{
			if( !type_check( node->child->brother ) )
				yysemerror( node, STR_NO_RETURN );

			break;
		}

		case N_STAT_LIST:
		{
			Node* current_node = node->child;
			while( current_node != NULL && !has_return )
			{
				// Keeping track of the return statement
				has_return |= type_check( current_node );

				current_node = current_node->brother;
			}

			if( has_return && current_node != NULL )
				yysemerror( current_node, STR_CODE_AFTER_RETURN );

			break;
		}

		case N_ASSIGN_STAT:
		{
			Schema* result = malloc( sizeof( Symbol ) );

			result = infere_lhs_schema( node->child, TRUE );
			
			if( result == NULL )
				yysemerror( node, PRINT_ERROR( STR_UNDECLARED, "assignment" ) );
			
			simplify_expression( node->child->brother );
			if( result->type != infere_expression_schema( node->child->brother )->type )
				yysemerror( node, STR_ASSIGN_TYPE );

			break;
		}

		case N_IF_STAT:
		{
			// Checking if the type of the conditional expression is a boolean
			simplify_expression( node->child );
			if( infere_expression_schema( node->child )->type != TS_BOOL )
				yysemerror( node, STR_COND_EXPR );

			has_return = TRUE;
			Node* current_node;
			
			for( current_node = node->child->brother;
				 current_node->brother != NULL;
				 current_node = current_node->brother )
				has_return &= type_check( current_node );

			// Checking if the last brother of the if is an else
			if( current_node->value.n_val == N_ELSE_STAT )
				has_return &= type_check( current_node );
			else
			{
				type_check( current_node );
				has_return = FALSE;
			}

			break;
		}

		case N_ELSIF_STAT:
		{
			simplify_expression( node->child );
			if( infere_expression_schema( node->child )->type != TS_BOOL )
				yysemerror( node->child, STR_COND_EXPR );

			has_return = TRUE;
			Node* current_node;
			for( current_node = node->child->brother;
				 current_node != NULL;
				 current_node = current_node->brother )
				has_return &= type_check( current_node );

			break;
		}

		case N_ELSE_STAT:
		{
			has_return = type_check( node->child );
			break;
		}

		case N_WHILE_STAT:
		{
			simplify_expression( node->child );
			if( infere_expression_schema( node->child )->type != TS_BOOL )
				yysemerror( node, STR_COND_EXPR );
			
			// NOTE: A possible optimisation could be, if the while-condition
			// simplifies to TRUE, to check if there exists inside the
			// statement list a return statement. If it does not, this is an
			// infinite loop and therefore the user should be notified.
			Node* current_node = node->child->brother;
			while( current_node != NULL )
			{
				type_check( current_node );
				current_node = current_node->brother;
			}
		    break;
		}

		case N_FOR_STAT:
		{
			Node* current_node = node->child;
			Symbol* iterable_variable = fetch_scope( current_node->value.s_val );
			if( iterable_variable == NULL )
				yysemerror( current_node, PRINT_ERROR( STR_UNDECLARED, "for" ) );
			ClassSymbol iterable_variable_class = iterable_variable->clazz;
			switch( iterable_variable_class )
			{
				case CS_VAR:
				case CS_PAR:
					iterable_variable->clazz = CS_CONST;
					break;

				default:
					yysemerror( current_node, PRINT_ERROR( STR_WRONG_CLASS, "not a variable" ) );
			}
			if( iterable_variable->schema->type != TS_INT )
				yysemerror( current_node, PRINT_ERROR( STR_CONFLICT_TYPE, "expected integer" ) );

			simplify_expression( current_node = current_node->brother );
			if( infere_expression_schema( current_node )->type != TS_INT )
				yysemerror( current_node, PRINT_ERROR( STR_CONFLICT_TYPE, "expected integer" ) );
			simplify_expression( current_node = current_node->brother );
			if( infere_expression_schema( current_node )->type != TS_INT )
				yysemerror( current_node, PRINT_ERROR( STR_CONFLICT_TYPE, "expected integer" ) );

			for( current_node = current_node->brother; current_node != NULL; current_node = current_node->brother )
				type_check( current_node );

			iterable_variable->clazz = iterable_variable_class;
			break;
		}

		case N_FOREACH_STAT:
		{
			Node* current_node = node->child;
			Symbol* iterable_variable = fetch_scope( current_node->value.s_val );
			if( iterable_variable == NULL )
				yysemerror( current_node, PRINT_ERROR( STR_UNDECLARED, "foreach" ) );
			switch( iterable_variable->clazz )
			{
				case CS_VAR:
				case CS_PAR:
					break;

				default:
					yysemerror( current_node, PRINT_ERROR( STR_WRONG_CLASS, "not a variable" ) );
			}

			simplify_expression( current_node = current_node->brother );
			Schema* iterated_expression = infere_expression_schema( current_node );
			if( iterated_expression->type != TS_VECTOR || !schema_check( iterable_variable->schema, iterated_expression->child ) )
				yysemerror( current_node, PRINT_ERROR( STR_CONFLICT_TYPE, "expected vector of the same type as the iterable variable" ) );

			for( current_node = current_node->brother; current_node != NULL; current_node = current_node->brother )
				type_check( current_node );

			break;
		}

		case N_RETURN_STAT:
		{
			// Looking the return type of the actual function and check if it's
			// equal to the type of of the return expression.
			// ☆*:.｡. o(≧▽≦)o .｡.:*☆ 
			simplify_expression( node->child );
			if( !schema_check( infere_expression_schema( node->child ), ( (Symbol*) scope->function )->schema ) )
				yysemerror( node, STR_RETURN_TYPE );

			has_return = TRUE;
			break;
		}

		case N_READ_STAT:
		{
			Node* id_node;
			if( node->child->brother != NULL )
			{
				id_node = node->child->brother;

				simplify_expression( node->child );
				if( infere_expression_schema( node->child )->type != TS_STRING )
					yysemerror( node->child, PRINT_ERROR( STR_CONFLICT_TYPE, "expected string" ) );
			}
			else
				id_node = node->child;

			Symbol* destination_variable = fetch_scope( id_node->value.s_val );
			if( destination_variable == NULL )
				yysemerror( id_node, PRINT_ERROR( STR_UNDECLARED, "read" ) );
			switch( destination_variable->clazz )
			{
				case CS_VAR:
				case CS_PAR:
					break;

				default:
					yysemerror( id_node, PRINT_ERROR( STR_WRONG_CLASS, "not a variable" ) );
			}

			break;
		}

		case N_WRITE_STAT:
		{
			Node* expr_node;
			if( node->child->brother != NULL )
			{
				expr_node = node->child->brother;

				simplify_expression( node->child );
				if( infere_expression_schema( node->child )->type != TS_STRING )
					yysemerror( node->child, PRINT_ERROR( STR_CONFLICT_TYPE, "expected string" ) );
			}
			else
				expr_node = node->child;

			simplify_expression( node->child );
			infere_expression_schema( expr_node );
			break;
		}

		case N_EXPR:
		case N_BOOL_TERM:
		case N_REL_TERM:
		case N_LOW_TERM:
		case N_ELSIF_EXPR:
		case N_FUNC_CALL:
		case N_COND_EXPR:
		case N_DYNAMIC_INPUT:
		case N_DYNAMIC_OUTPUT:
			break;
	}

	return has_return;
}

/**
 * @brief Searches for an ID in the actual scope and, eventually, its fathers.
 *
 * @param id
 *
 * @return Returns the Symbol* found or NULL.
 */
Symbol* fetch_scope( char* id )
{
	Symbol* result = NULL;

	stacklist current_scope = scope;
	while( current_scope->next != NULL )
	{
		if( hashmap_get( ( (Symbol*) current_scope->function )->locenv, id, (any_t*) &result ) == MAP_OK )
			return result;
		if( ( (Symbol*) current_scope->function )->name == id )
			return (Symbol*) current_scope->function;

		current_scope = current_scope->next;
	}

	if( hashmap_get( ( (Symbol*) current_scope->function )->locenv, id, (any_t*) &result ) != MAP_OK )
		if( ( (Symbol*) current_scope->function )->name == id )
			result = (Symbol*) current_scope->function;

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
	any_t useless;
	if( ( (Symbol*) scope->function )->name == element->name
		|| hashmap_get( ( (Symbol*) scope->function )->locenv, element->name, &useless ) == MAP_OK )
		return FALSE;

	hashmap_put( ( (Symbol*) scope->function )->locenv, element->name, element );
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

	if( node->type == T_ID )
		fprintf( stdout, "Line %d on symbol '%s': %s.\n", node->line, node->value.s_val, type );
	else
	{
		tree_print( node, 0 );
		fprintf( stdout, "Line %d: %s.\n", node->line, type );
	}

	exit( 3 );
}
