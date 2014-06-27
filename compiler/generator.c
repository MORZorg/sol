#include "generator.h"
#include "tree_print.h"

#define STR_COMPILER_BUG "Compiler bug."

extern FILE* yyin;
extern Node* root;
extern Symbol* symbol_table;
extern stacklist scope;
extern char* CODE_OPERATORS[];

FILE* yyin;

/*
 * PRIVATE FUNCTION
 */

Stat* new_stat( Operator op )
{
	Stat* result = malloc( sizeof( Stat ) );
	result->address = 0;
	result->op = op;
	result->next = NULL;

	return result;
}

/*
 * PUBLIC FUNCTIONS
 */

/**
 * @brief Function that starts the code generation
 *
 * @param input Input file where we will read every statement
 * @param output Output code file
 *
 * @return Returns 0 if all is done correctly.
 */
int yygen( FILE* input, FILE* output )
{
	yyin = input;

	int sem_result = yysem();
	if( sem_result != 0 )
		return sem_result;

	stacklist_push( &scope, (stacklist_t) symbol_table );

	// Instantiate an hashmap, used to track the function definitions inside
	// the p-code
	func_map = hashmap_new();

	// The intro code is generated afterwards because it needs the updated
	// symbol table including the additional temporary variables.
	Code result = generate_code( root );
	result = append_code( generate_intro_code( symbol_table ),
						  result );

	output_code( output, result );

	return 0;
}

/**
 * @brief Function that generates the initial part of the generated code
 *
 * @param base_function The symbol that represents the first function of the program
 *
 * @return Returns the generated code
 */
Code generate_intro_code( Symbol* base_function )
{
	Code result = empty_code();

	int i;
	for( i = 0; i < base_function->formals_size; i++ )
	{
		result = append_code( result,
								make_code_string_param( SOL_RD, schema_to_string( base_function->formals[ i ]->schema ) ) );
	}

	Code goto_stat = make_code_no_param( SOL_GOTO );
	result = concatenate_code( 5,
							   result,
							   make_code_two_param( SOL_PUSH, hashmap_length( base_function->locenv ), -1 ),
							   goto_stat,
							   make_code_no_param( SOL_POP ),
							   make_code_no_param( SOL_HALT ) );
	goto_stat.head->args[ 0 ].i_val = result.size;

	return result;
}

/**
 * @brief Function that, given a node of the program, generates the corresponding S-code.
 *
 * @param node Node to be processed
 *
 * @return Returns the generated code
 */
Code generate_code( Node* node )
{
	Code result = empty_code();

	switch( node->type )
	{
		case T_INT_CONST:
			result = make_ldi( node->value.i_val );
			break;

		case T_CHAR_CONST:
			result = make_ldc( node->value.s_val );
			break;

		case T_REAL_CONST:
			result = make_ldr( node->value.r_val );
			break;

		case T_STR_CONST:
			result = make_lds( node->value.s_val );
			break;

		case T_BOOL_CONST:
			result = make_ldc( node->value.b_val == TRUE ? "1" : "0" );
			break;
			
		case T_ID:
			result = generate_lhs_code( node, NULL, FALSE );
			break;

		case T_INSTANCE_EXPR:
		{
			Node* current_value = node->child;
			int size = 0;
			while( current_value != NULL )
			{
				result = append_code( result, generate_code( current_value ) );
				size++;

				current_value = current_value->brother;
			}

			result = append_code( result, make_code_two_param( SOL_CAT, size, schema_size( infere_expression_schema( node ) ) ) );
			break;
		}
		
		case T_BUILT_IN_CALL:
			switch( node->value.q_val )
			{
				case Q_TOINT:
					result = append_code( generate_code( node->child ), make_code_no_param( SOL_TOINT ) );
					break;
					
				case Q_TOREAL:
					result = append_code( generate_code( node->child ), make_code_no_param( SOL_TOREAL ) );
					break;

				default:
					break;
			}
			break;

		case T_LOGIC_EXPR:
		{
			Node* current_child = node->child;

			Code expr1 = generate_code( current_child );
			Code expr2 = generate_code( current_child = current_child->brother );

			switch( node->value.q_val )
			{
				case Q_AND:
					// Expr1 evaluation
					result = append_code( result, expr1 );
					// The jump must surpass the expr2 code and the following unconditioned jump, therefore it is at size + 2 
					result = append_code( result, make_code_one_param( SOL_JMF, expr2.size + 2 ) );
					// Expr2 evaluation
					result = append_code( result, expr2 );
					// If true, surpass the ldc setting the result to false and go to the one which sets it as true
					result = append_code( result, make_code_one_param( SOL_JMP, 2 ) );
					// False ldc
					result = append_code( result, make_ldc( "0" ) );
					// The result is now on top of the stack (hopefully, if Lamperti is right)
					break;

				case Q_OR:
					// Expr1 evaluation
					result = append_code( result, expr1 );
					// If expr1 is false, jump to expr2 evaluation
					result = append_code( result, make_code_one_param( SOL_JMF, 3 ) );
					// Else load true and jump after expr2 evaluation
					result = append_code( result, make_ldc( "1" ) );
					result = append_code( result, make_code_one_param( SOL_JMP, expr2.size + 1 ) );
					// Expr2 evaluation
					result = append_code( result, expr2 );
					// The result is now on top of the stack (hopefully, if Lamperti is right)
					break;

				default:
					break;
			}

			break;
		}

		case T_REL_EXPR:
		{
			// Process the 2 expressions, operands of the rel_expr
			Node* current_child = node->child;
			Code expr1 = generate_code( current_child );
			Code expr2 = generate_code( current_child = current_child->brother );

			Schema* operands_schema = infere_expression_schema( current_child );
			Operator use_me;

			// Determine the operand to use using both the qualifier and operands type
			switch( node->value.q_val )
			{
				case Q_EQ:
					use_me = SOL_EQU;
					break;

				case Q_NEQ:
					use_me = SOL_NEQ;
					break;

				case Q_GT:
					switch( operands_schema->type )
					{
						case TS_CHAR:
							use_me = SOL_CGT;
							break;

						case TS_INT:
							use_me = SOL_IGT;
							break;

						case TS_REAL:
							use_me = SOL_RGT;
							break;

						case TS_STRING:
							use_me = SOL_SGT;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;
						
				case Q_GEQ:
					switch( operands_schema->type )
					{
						case TS_CHAR:
							use_me = SOL_CGE;
							break;

						case TS_INT:
							use_me = SOL_IGE;
							break;

						case TS_REAL:
							use_me = SOL_RGE;
							break;

						case TS_STRING:
							use_me = SOL_SGE;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;
						
				case Q_LT:
					switch( operands_schema->type )
					{
						case TS_CHAR:
							use_me = SOL_CLT;
							break;

						case TS_INT:
							use_me = SOL_ILT;
							break;

						case TS_REAL:
							use_me = SOL_RLT;
							break;

						case TS_STRING:
							use_me = SOL_ILT;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;
						
				case Q_LEQ:
					switch( operands_schema->type )
					{
						case TS_CHAR:
							use_me = SOL_CLE;
							break;

						case TS_INT:
							use_me = SOL_ILE;
							break;

						case TS_REAL:
							use_me = SOL_RLE;
							break;

						case TS_STRING:
							use_me = SOL_SLE;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;

				case Q_IN:
					use_me = SOL_IN;
					break;
						
				default:
					printf(STR_COMPILER_BUG);
					exit(1);
					break;
			}

			// Pretty standard, really
			result = concatenate_code( 4, result, expr1, expr2, make_code_no_param( use_me ) );

			break;
		}

		case T_MATH_EXPR:
		{
			Node* current_child = node->child;
			
			// Similar to REL_EXPR, but with less cases
			Code expr1 = generate_code( current_child );
			Code expr2 = generate_code( current_child = current_child->brother );

			Schema* operands_schema = infere_expression_schema( current_child );
			Operator use_me;

			// Determine the operand to use using both the qualifier and operands type
			switch( node->value.q_val )
			{
				case Q_PLUS:
					switch( operands_schema->type )
					{
						case TS_INT:
							use_me = SOL_IPLUS;
							break;

						case TS_REAL:
							use_me = SOL_RPLUS;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;
						
				case Q_MINUS:
					switch( operands_schema->type )
					{
						case TS_INT:
							use_me = SOL_IMINUS;
							break;

						case TS_REAL:
							use_me = SOL_RMINUS;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;
				
				case Q_MULTIPLY:
					switch( operands_schema->type )
					{
						case TS_INT:
							use_me = SOL_ITIMES;
							break;

						case TS_REAL:
							use_me = SOL_RTIMES;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;

				case Q_DIVIDE:
					switch( operands_schema->type )
					{
						case TS_INT:
							use_me = SOL_IDIV;
							break;

						case TS_REAL:
							use_me = SOL_RDIV;
							break;

						default:
							printf(STR_COMPILER_BUG);
							exit(1);
							break;
					}
					break;

				default:
					printf(STR_COMPILER_BUG);
					exit(1);
					break;
			}
			
			// Pretty standard, really
			result = concatenate_code( 4, result, expr1, expr2, make_code_no_param( use_me ) );
			
			break;
		}

		case T_NEG_EXPR:
		{
			Node* current_child = node->child;
			
			Code expr = generate_code( current_child );

			Schema* operands_schema = infere_expression_schema( current_child );
			Operator use_me;

			switch( operands_schema->type )
			{
				case TS_INT:
					use_me = SOL_IUMI;
					break;

				case TS_REAL:
					use_me = SOL_RUMI;
					break;

				case TS_BOOL:
					use_me = SOL_NEG;
					break;

				default:
					printf(STR_COMPILER_BUG);
					exit(1);
					break;
			}

			// Pretty standard, really
			result = concatenate_code( 3, result, expr, make_code_no_param( use_me ) );
			
			break;
		}

		case T_UNQUALIFIED_NONTERMINAL:
			switch( node->value.n_val )
			{
				case N_STAT_LIST:
				{
					Node* current_child;
					for( current_child = node->child; current_child != NULL; current_child = current_child->brother )
					{
						result = append_code( result, generate_code( current_child ) );
					}

					break;
				}

				case N_FUNC_DECL:
				{
					Node* current_node = node->child;

					Symbol* func_scope = fetch_scope( current_node->value.s_val );
					
					result = make_code_one_param( SOL_FUNC, func_scope->oid );

					// TODO Find a way to avoid to duplicate the base function.
					stacklist_push( &scope, (stacklist_t) func_scope );
					current_node = current_node->brother;

					Code entry_code = empty_code();
					
					// If PAR_LIST is missing, current_node will be the domain
					// node, better to check also the type to avoid collisions
					// between n_val and q_val.
					if( current_node->type == T_UNQUALIFIED_NONTERMINAL
						&& current_node->value.n_val == N_PAR_LIST )
					{
						entry_code = append_code( entry_code, generate_code( current_node ) );
						current_node = current_node->brother;
					}
					// Skip the return type
					current_node = current_node->brother;

					// No generation needed
					if( current_node->value.n_val == N_TYPE_SECT )
						current_node = current_node->brother;
					
					if( current_node->value.n_val == N_VAR_SECT )
					{
						entry_code = append_code( entry_code, generate_code( current_node ) );
						current_node = current_node->brother;
					}
					
					if( current_node->value.n_val == N_CONST_SECT )
					{
						entry_code = append_code( entry_code, generate_code( current_node ) );
						current_node = current_node->brother;
					}

					Node* function_list = NULL;

					// The FUNC_LIST processing is done after the body
					if( current_node->value.n_val == N_FUNC_LIST )
					{
						function_list = current_node;

						current_node = current_node->brother;
					}
					
					entry_code = append_code( entry_code, generate_code( current_node ) );

					// Create new entry in func_map with the function's oid as key
					char key[ MAX_INT_LEN ];
					sprintf( key, "%d", func_scope->oid );

					FuncDesc* description = malloc( sizeof( FuncDesc ) );

					description->size = hashmap_length( func_scope->locenv );
					description->scope = func_scope->nesting;
					description->entry = entry_code.head->address;
					
					hashmap_put( func_map, key, description );

					result = append_code( result, entry_code );
					
					if( function_list != NULL )
					{
						Node* current_child = function_list->child;

						while( current_child != NULL )
						{
							
							result = append_code( result, generate_code( current_child ) );
							current_child = current_child->brother;
						}
					}

					break;
				}

				case N_FUNC_CALL:
				{
					Node* current_child = node->child;

					char key[20];

					sprintf( key, "%d", fetch_scope( current_child->value.s_val )->oid );

					// Retrieve and compute all the parameters
					while( ( current_child = current_child->brother ) != NULL )
						result = append_code( result, generate_code( current_child ) );

					FuncDesc* description;

					hashmap_get( func_map, key, (any_t*) &description );

					// FIXME Shouldn't be checked, only done to avoid debugging crashes
					if( description != NULL )
					{
						// The number of objects defined in the function's activation record
						int size = description->size;
                        // Since description->scope contains the nesting in
                        // which the function is defined, theoretically the
                        // following value should represent the distance
                        // between the actual call nesting and the definition
                        // nesting
						int chain = ( (Symbol*) scope->function )->nesting - description->scope + 1;
						// The address of the function's body start
						int entry = description->entry;
					
						result = concatenate_code( 4,
												   result,
												   make_code_two_param( SOL_PUSH, size, chain ),
												   make_code_one_param( SOL_GOTO, entry ),
												   make_code_no_param( SOL_POP ) );
					}

					break;
				}
				
				case N_PAR_LIST:
				{
					Node* current_child = node->child;
					
					Code news_code = empty_code();
					Code stos_code = empty_code();

					while( current_child != NULL )
					{
						Node* current_par = current_child->child;

						while( current_par->brother != NULL )
						{
							news_code = append_code( news_code, make_decl( fetch_scope( current_par->value.s_val )->schema ) );

							stos_code = append_code( generate_lhs_code( current_par, NULL, TRUE ),
                                                     stos_code );

							current_par = current_par->brother;
						}

						current_child = current_child->brother;
					}

					result = concatenate_code( 3, result, news_code, stos_code );

					break;
				}

				case N_VAR_SECT:
				{
					Node* current_child = node->child;
					while( current_child != NULL )
					{
						Node* current_id = current_child->child;
						while( current_id->brother != NULL )
						{
							result = append_code( result, make_decl( fetch_scope( current_id->value.s_val )->schema ) );

							current_id = current_id->brother;
						}

						current_child = current_child->brother;
					}
					break;
				}

				case N_CONST_SECT:
				{
					Code values = empty_code();
					Node* current_child = node->child;
					while( current_child != NULL )
					{
						int const_size = 0;
						stacklist const_list;

						Node* current_id = current_child->child;
						while( current_id->brother->brother != NULL )
						{
							result = append_code( result, make_decl( fetch_scope( current_id->value.s_val )->schema ) );
							const_size++;

							stacklist_push( &const_list, (stacklist_t) current_id );

							current_id = current_id->brother;
						}

						for( ; const_size > 0; const_size-- )
						{
							values = concatenate_code( 3,
													   values,
													   generate_code( current_id->brother ),
													   generate_lhs_code( (Node*) const_list->function, NULL, TRUE ) );
							
							stacklist_pop( &const_list );
						}

						current_child = current_child->brother;
					}

					result = append_code( result, values );
					break;
				}
				
				case N_FUNC_LIST:
				{
					Node* current_child = node->child;

					while( current_child != NULL )
					{
						result = append_code( result, generate_code( current_child ) );

						current_child = current_child->brother;
					}

					break;
				}

				// Ignore the id, from there on, it's just a stat list
				case N_FUNC_BODY:
				{
					result = generate_code( node->child->brother );

					break;
				}
				
				case N_RETURN_STAT:
				{
					Code returned_stuff = empty_code();

					if( node->child != NULL )
						returned_stuff = append_code( returned_stuff, generate_code( node->child ) );

					result = concatenate_code( 3, result, returned_stuff, make_code_no_param( SOL_RETURN ) );

					break;
				}

				case N_ASSIGN_STAT:
				{
					Code assignment = generate_lhs_code( node->child, NULL, TRUE );
					if( assignment.size == 1 )
						result = append_code( generate_code( node->child->brother ), assignment );
					else
					{
						Stat* pre_tail;
						for( pre_tail = assignment.head;
							 pre_tail->next != assignment.tail;
							 pre_tail = pre_tail->next );

						Code tail = empty_code();
						tail.head = tail.tail = assignment.tail;
						tail.size = 1;

						assignment.tail = pre_tail;
						assignment.size -= 1;
						pre_tail->next = NULL;

						result = concatenate_code( 3,
												   assignment,
												   generate_code( node->child->brother ),
												   tail );
					}
					break;
				}

				case N_COND_EXPR:
				{
					Node* current_child = node->child;
					Code condition = generate_code( current_child );
					Code value = generate_code( current_child = current_child->brother );
					Code next = make_code_one_param( SOL_JMF, value.size + 2 );
					Code exit = make_code_no_param( SOL_JMP );
					result = concatenate_code( 4, condition, next, value, exit );
					
					stacklist exit_list = NULL;
					stacklist_push( &exit_list, (void*) exit.head );
					
					while( ( current_child = current_child->brother )->value.n_val == N_ELSIF_EXPR )
					{
						condition = generate_code( current_child->child );
						value = generate_code( current_child->child->brother );
						next = make_code_one_param( SOL_JMF, value.size + 2 );
						exit = make_code_no_param( SOL_JMP );
						result = concatenate_code( 5, result, condition, next, value, exit );
						
						stacklist_push( &exit_list, (void*) exit.head );
					}
					
					result = append_code( result, generate_code( current_child ) );
					
					Stat* exit_stat;
					while( exit_list != NULL )
					{
						exit_stat = exit_list->function;
						exit_stat->args[ 0 ].i_val = result.size - exit_stat->address;

						stacklist_pop( &exit_list );
					}
					
					break;
				}

				case N_IF_STAT:
				{
					Node* current_child = node->child;
					Code condition = generate_code( current_child );
					Code value = generate_code( current_child = current_child->brother );
					Code next = make_code_one_param( SOL_JMF, value.size + 2 );
					Code exit = make_code_no_param( SOL_JMP );
					result = concatenate_code( 4, condition, next, value, exit );

					stacklist exit_list = NULL;
					stacklist_push( &exit_list, (void*) exit.head );
					
					while( ( current_child = current_child->brother ) != NULL
						   && current_child->value.n_val == N_ELSIF_STAT )
					{
						condition = generate_code( current_child->child );
						value = generate_code( current_child->child->brother );
						next = make_code_one_param( SOL_JMF, value.size + 2 );
						exit = make_code_no_param( SOL_JMP );
						result = concatenate_code( 5, result, condition, next, value, exit );
						
						stacklist_push( &exit_list, (void*) exit.head );
					}
					
					if( current_child != NULL )
						result = append_code( result, generate_code( current_child->child ) );
					
					Stat* exit_stat;
					while( exit_list != NULL )
					{
						exit_stat = exit_list->function;
						exit_stat->args[ 0 ].i_val = result.size - exit_stat->address;

						stacklist_pop( &exit_list );
					}
					
					break;
				}

				case N_WHILE_STAT:
				{
					Code exit = make_code_no_param( SOL_JMF );
					Code up = make_code_no_param( SOL_JMP );

					result = concatenate_code( 4,
											   generate_code( node->child ), exit,
											   generate_code( node->child->brother ), up );

					exit.head->args[ 0 ].i_val = result.size - exit.head->address;
					up.head->args[ 0 ].i_val = -up.head->address;
					break;
				}

				case N_FOR_STAT:
				{
					// Transform the FOR into a WHILE, then delegate
					// TODO Make sure the name is generated correctly
					// Also the nesting
					Symbol* temp_var = malloc( sizeof( Symbol ) );
					temp_var->name = malloc( MAX_INT_LEN * sizeof( char ) );
					sprintf( temp_var->name, "%d", hashmap_length( ( (Symbol*) scope->function )->locenv ) );
					temp_var->oid = hashmap_length( ( (Symbol*) scope->function )->locenv );
					temp_var->nesting = ( (Symbol*) scope->function )->nesting;
					temp_var->clazz = CS_VAR;
					temp_var->schema = fetch_scope( node->child->value.s_val )->schema;
					insert_unconflicted_element( temp_var );

					Node* init_node = new_nonterminal_node( N_ASSIGN_STAT );
					init_node->child = new_terminal_node( T_ID, node->child->value );
					init_node->child->brother = node->child->brother;

					Value temp_val = { .s_val = temp_var->name };
					Node* temp_def_node = new_nonterminal_node( N_ASSIGN_STAT );
					temp_def_node->child = new_terminal_node( T_ID, temp_val );
					temp_def_node->child->brother = node->child->brother->brother;
					init_node->brother = temp_def_node;

					Node* while_node = new_nonterminal_node( N_WHILE_STAT );
					while_node->child = new_qualified_node( T_REL_EXPR, Q_LT );
					while_node->child->child = new_terminal_node( T_ID, node->child->value );
					while_node->child->child->brother = new_terminal_node( T_ID, temp_val );
					while_node->child->brother = node->child->brother->brother->brother;
					temp_def_node->brother = while_node;

					Value increment = { .i_val = 1 };
					Node* loop_node = new_nonterminal_node( N_ASSIGN_STAT );
					loop_node->child = new_terminal_node( T_ID, node->child->value );
					loop_node->child->brother = new_qualified_node( T_MATH_EXPR, Q_PLUS );
					loop_node->child->brother->child = new_terminal_node( T_ID, node->child->value );
					loop_node->child->brother->child->brother = new_terminal_node( T_INT_CONST, increment );
					assign_brother( &( node->child->brother->brother->brother->child ), loop_node );

					node->value.n_val = N_STAT_LIST;
					node->child->brother->brother->brother = NULL;
					node->child->brother->brother = NULL;
					node->child->brother = NULL;
					node->child = init_node;

					result = generate_code( node );
					break;
				}

				case N_FOREACH_STAT:
				{
					// Transform the FOREACH into a FOR, then delegate
					// TODO Make sure the name is generated correctly
					// Also the nesting
					Symbol* temp_var = malloc( sizeof( Symbol ) );
					temp_var->name = malloc( MAX_INT_LEN * sizeof( char ) );
					sprintf( temp_var->name, "%d", hashmap_length( ( (Symbol*) scope->function )->locenv ) );
					temp_var->oid = hashmap_length( ( (Symbol*) scope->function )->locenv );
					temp_var->nesting = ( (Symbol*) scope->function )->nesting;
					temp_var->clazz = CS_VAR;
					temp_var->schema = infere_expression_schema( node->child->brother );
					insert_unconflicted_element( temp_var );

					Symbol* loop_var = malloc( sizeof( Symbol ) );
					loop_var->name = malloc( MAX_INT_LEN * sizeof( char ) );
					sprintf( loop_var->name, "%d", hashmap_length( ( (Symbol*) scope->function )->locenv ) );
					loop_var->oid = hashmap_length( ( (Symbol*) scope->function )->locenv );
					loop_var->nesting = ( (Symbol*) scope->function )->nesting;
					loop_var->clazz = CS_VAR;
					loop_var->schema = malloc( sizeof( Schema ) );
					loop_var->schema->type = TS_INT;
					insert_unconflicted_element( loop_var );

					Value temp_val = { .s_val = temp_var->name };
					Value loop_val = { .s_val = loop_var->name };
					Value loop_from = { .i_val = 0 };
					Value loop_to = { .i_val = temp_var->schema->size };

					Node* init_node = new_nonterminal_node( N_ASSIGN_STAT );
					init_node->child = new_terminal_node( T_ID, temp_val );
					init_node->child->brother = node->child->brother;

					Node* for_node = new_nonterminal_node( N_FOR_STAT );
					for_node->child = new_terminal_node( T_ID, loop_val );
					for_node->child->brother = new_terminal_node( T_INT_CONST, loop_from );
					for_node->child->brother->brother = new_terminal_node( T_INT_CONST, loop_to );
					for_node->child->brother->brother->brother = node->child->brother->brother;
					init_node->brother = for_node;

					Node* loop_node = new_nonterminal_node( N_ASSIGN_STAT );
					loop_node->child = new_terminal_node( T_ID, node->child->value );
					loop_node->child->brother = new_nonterminal_node( N_FIELDING );
					loop_node->child->brother->child = new_terminal_node( T_ID, temp_val );
					loop_node->child->brother->child->brother = new_terminal_node( T_ID, loop_val );
					loop_node->brother = node->child->brother->brother->child;
					node->child->brother->brother->child = loop_node;

					node->value.n_val = N_STAT_LIST;
					node->child->brother->brother->brother = NULL;
					node->child->brother->brother = NULL;
					node->child->brother = NULL;
					node->child = init_node;

					result = generate_code( node );
					break;
				}
				
				case N_READ_STAT:
				{
					Symbol* referenced_id;
					if( node->child->brother == NULL )
					{
						referenced_id = fetch_scope( node->child->value.s_val );
						result = make_code_two_param( SOL_READ, ( (Symbol*) scope->function )->nesting - referenced_id->nesting, referenced_id->oid );
					}
					else
					{
						referenced_id = fetch_scope( node->child->brother->value.s_val );
						result = append_code( generate_code( node->child ),
											  make_code_two_param( SOL_FREAD, ( (Symbol*) scope->function )->nesting - referenced_id->nesting, referenced_id->oid ) );
					}
						
					result.tail->args[ 2 ].s_val = schema_to_string( referenced_id->schema );
						
					break;
				}

				case N_DYNAMIC_INPUT:
				{
					Node* child = node->child;

					Operator use_me = SOL_RD;

					// Determine if the WR has a specifier-opt or not
					if( child->brother != NULL )
					{
						result = append_code( result, generate_code( child ) );

						child = child->brother;

						use_me = SOL_FRD;
					}
					
					result = append_code( result, make_code_string_param( use_me, schema_to_string( infere_expression_schema( node ) ) ) );

					break;
				}

				case N_DYNAMIC_OUTPUT:
				{
					Node* expr_child = node->child;

					Operator use_me = SOL_WR;

					// Determine if the WR has a specifier-opt or not
					if( expr_child->brother != NULL )
					{
						result = append_code( result, generate_code( expr_child ) );

						expr_child = expr_child->brother;

						use_me = SOL_FWR;
					}
					
					Code expr = generate_code( expr_child );

					result = concatenate_code( 3, result, expr, make_code_string_param( use_me, schema_to_string( infere_expression_schema( node ) ) ) );

					break;
				}

				case N_WRITE_STAT:
				{
					Operator op;
					Schema* output_schema;
					if( node->child->brother == NULL )
					{
						output_schema = infere_expression_schema( node->child );
						op = SOL_WRITE;
						result = generate_code( node->child );
					}
					else
					{
						output_schema = infere_expression_schema( node->child->brother );
						op = SOL_FWRITE;
						result = append_code( generate_code( node->child ),
											  generate_code( node->child->brother ) );
					}

					result = append_code( result,
										  make_code_string_param( op, schema_to_string( output_schema ) ) );
						
					break;
				}

				case N_FIELDING:
				case N_INDEXING:
					result = generate_lhs_code( node, NULL, FALSE );
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return result;
}

/**
 * @brief Function that generates the code for a lhs term, which could be a combination of fields and arrays.
 *
 * @param node 
 * @param is_first Indicates if the node is the first or a nested one.
 *
 * @return The whole code of the lhs
 */
Code generate_lhs_code( Node* node, Schema** id_schema, Boolean is_assigned )
{
	Code result = empty_code();

	switch( node->type )
	{
		case T_ID:
		{
			Symbol* referenced_id = fetch_scope( node->value.s_val );

			Operator op;
			if( id_schema == NULL )
			{
				if( is_assigned )
					op = SOL_STO;
				else
					op = SOL_LOD;
			}
			else
			{
				op = SOL_LDA;
				*(id_schema) = referenced_id->schema;
			}

			result = make_code_two_param( op, ( (Symbol*) scope->function )->nesting - referenced_id->nesting, referenced_id->oid );

			break;
		}

		case T_UNQUALIFIED_NONTERMINAL:
		{
			switch( node->value.n_val )
			{
				case N_FIELDING:
				{
					Node* current_node = node->child;
					// Recursivity on the first child
					Schema* child_schema = NULL;
					result = generate_lhs_code( current_node, &(child_schema), is_assigned );

					child_schema = child_schema->child;
					current_node = current_node->brother;

					// Finding the field of the struct that we are looking for, adding the shift for the previous fields
					int fda_shift = 0;
					while( child_schema->brother != NULL && child_schema->id != current_node->value.s_val )
					{
						fda_shift += schema_size( child_schema );
						child_schema = child_schema->brother;
					}
					if( fda_shift > 0 )
						result = append_code( result, make_code_one_param( SOL_FDA, fda_shift ) );

					if( id_schema == NULL )
					{
						Operator op;
						if( is_assigned )
							op = SOL_IST;
						else
							switch( child_schema->type )
							{
								case TS_STRUCT:
								case TS_VECTOR:
									op = SOL_SIL;

								default:
									op = SOL_EIL;
							}

						result = append_code( result, make_code_one_param( op, schema_size( child_schema ) ) );
					}
					else
						*id_schema = child_schema->child;

					break;
				}

				case N_INDEXING:
				{
					Node* current_node = node->child;

					// Recursivity on the first child
					Schema* array_schema = NULL;
					result = generate_lhs_code( current_node, &(array_schema), is_assigned );

					/* fprintf( stderr, "Found schema: " ); */
					/* schema_print( array_schema ); */
					/* fprintf( stdout, "\n" ); */

					current_node = current_node->brother;
					result = append_code( result, generate_code( current_node ) );

					result = append_code( result, make_code_one_param(
							SOL_IXA,
							schema_size( array_schema->child ) ) );

					if( id_schema == NULL )
					{
						if( is_assigned )
						{
							result = append_code( result, make_code_no_param( SOL_IST ) );
						}
						else
						{
							Operator op;
							switch( array_schema->type )
							{
								case TS_STRUCT:
								case TS_VECTOR:
									op = SOL_SIL;

								default:
									op = SOL_EIL;
							}

							result = append_code( result,
												  make_code_one_param( op, schema_size( array_schema->child ) ) );
						}
					}
					else
						*id_schema = array_schema->child;

					break;
				}

				default:
					fprintf( stderr, "ERROR: lhs code unknown unqualified node\n" );
					break;
			}
			break;
		}

		default:
			fprintf( stderr, "ERROR: lhs code unknown qualified node\n" );
			break;
	}

	return result;
}

/**
 * @brief Creates an empty code object, used to start a new code part.
 *
 * @return The empty code generated
 */
Code empty_code( void )
{
	static Code code = { NULL, 0, NULL };
	return code;
}

/**
 * @brief Given a specific code, it translates the address of every pieces of code by the given offset.
 *
 * @param code
 * @param offset
 */
void relocate_address( Code code, int offset )
{
	if( offset == 0 )
		return;

	Stat* current_stat;
	for( current_stat = code.head;
		 current_stat != NULL;
		 current_stat = current_stat->next )
		current_stat->address += offset;
}

/**
 * @brief Appends two code in a unique code part.
 *
 * @param first
 * @param second
 *
 * @return A single code object with the two given code parameters appended each other.
 */
Code append_code( Code first, Code second )
{
	if( first.size == 0 )
		return second;
	if( second.size == 0 )
		return first;

	if( second.head == NULL )
		fprintf( stderr, "TESTA VUOTA\n" );

	Code result;
	relocate_address( second, first.size - second.head->address );
	result.head = first.head;
	result.tail = second.tail;
	first.tail->next = second.head;
	result.size = first.size + second.size;

	return result;
}

/**
 * @brief Concatenates two or more code objects in a unique one.
 *
 * @param size The number of code objects that have to be merged together.
 * @param code1 
 * @param code2
 * @param ... All the other possibile code objects.
 *
 * @return The resulting code.
 */
Code concatenate_code( int size, Code code1, Code code2, ... )
{
	va_list argp;
	va_start( argp, code2 );
	Code result = append_code( code1, code2 );

	int i;
	for( i = 2; i < size; i++ )
		result = append_code( result, va_arg( argp, Code ) );

	va_end( argp );

	return result;
}

/**
 * @brief Creates the code for a no param code instance.
 *
 * @param op The operator.
 *
 * @return The code generated for that operator.
 */
Code make_code_no_param( Operator op )
{
	Code result;
	result.head = result.tail = new_stat( op );
	result.size = 1;

	return result;
}

/**
 * @brief Creates the code for a one integer parameter S-code statement.
 *
 * @param op 
 * @param arg The argument must be an integer.
 *
 * @return 
 */
Code make_code_one_param( Operator op, int arg )
{
	Code result = make_code_no_param( op );
	result.head->args[ 0 ].i_val = arg;

	return result;
}

/**
 * @brief Creates the code for a two integer parameters S-code parameter.
 *
 * @param op
 * @param arg1 This argument must be an integer.
 * @param arg2 This argument must be an integer.
 *
 * @return 
 */
Code make_code_two_param( Operator op, int arg1, int arg2 )
{
	Code result = make_code_no_param( op );
	result.head->args[ 0 ].i_val = arg1;
	result.head->args[ 1 ].i_val = arg2;

	return result;
}

/**
 * @brief Creates the code for a one string parameter S-code statement.
 *
 * @param op S-code operator.
 * @param arg1 String argument.
 *
 * @return Returns the generated code.
 */
Code make_code_string_param( Operator op, char* arg1 )
{
	Code result = make_code_no_param( op );

	result.head->args[ 0 ].s_val = arg1;

	return result;
}

/**
 * @brief 
 *
 * @param size
 * @param chain
 * @param entry
 *
 * @return 
 */
Code make_push_pop( int size, int chain, int entry )
{
	return concatenate_code( 3,
							 make_code_two_param( SOL_PUSH, size, chain ),
							 make_code_one_param( SOL_GOTO, entry ),
							 make_code_no_param( SOL_POP ) );
}

/**
 * @brief Creates the LDC S-code statement.
 *
 * @param a_char 
 *
 * @return 
 */
Code make_ldc( char* a_char )
{
	Code result;
	result = make_code_no_param( SOL_LDC );
	result.head->args[ 0 ].s_val = a_char;

	return result;
}

/**
 * @brief Creates the LDI S-code statement.
 *
 * @param an_int
 *
 * @return 
 */
Code make_ldi( int an_int )
{
	Code result;
	result = make_code_no_param( SOL_LDI );
	result.head->args[ 0 ].i_val = an_int;

	return result;
}

/**
 * @brief Creates the LDR S-code statement. 
 *
 * @param a_real
 *
 * @return 
 */
Code make_ldr( float a_real )
{
	Code result;
	result = make_code_no_param( SOL_LDR );
	result.head->args[ 0 ].r_val = a_real;

	return result;
}

/**
 * @brief Creates the LDS S-code statement. 
 *
 * @param a_string
 *
 * @return 
 */
Code make_lds( char* a_string )
{
	Code result;
	result = make_code_no_param( SOL_LDS );
	result.head->args[ 0 ].s_val = a_string;

	return result;
}

/**
 * @brief Creates the S-code corresponding to a variable declaration, knowing its schema.
 *
 * @param a_schema
 *
 * @return 
 */
Code make_decl( Schema* a_schema )
{
	Operator op;
	switch( a_schema->type )
	{
		case TS_VECTOR:
		case TS_STRUCT:
			op = SOL_NEWS;
			break;

		default:
			op = SOL_NEW;
			break;
	}

	return make_code_one_param( op, schema_size( a_schema ) );
}

/**
 * @brief Calculates the size of the given schema. Works also on structured schemas.
 *
 * @param a_schema
 *
 * @return Returns a size_t value of the given schema.
 */
size_t schema_size( Schema* a_schema )
{
	switch( a_schema->type )
	{
		case TS_CHAR:
			return sizeof( char );

		case TS_INT:
			return sizeof( int );

		case TS_REAL:
			return sizeof( float );

		case TS_STRING:
			return sizeof( char* );

		case TS_BOOL:
			return sizeof( char );

		case TS_STRUCT:
		{
			size_t size = 0;
			Schema* current_attr;
			for( current_attr = a_schema->child;
				 current_attr != NULL;
				 current_attr = current_attr->brother )
				size += schema_size( current_attr );

			return size;
		}

		case TS_VECTOR:
			return a_schema->size * schema_size( a_schema->child );

		case TS_ATTR:
			return schema_size( a_schema->child );

		default:
			return 0;
	}
}

/**
 * @brief Creates a string based on the given schema, indicating how it is made.
 *
 * @param a_schema
 *
 * @return Returns a string that describes the given schema.
 */
char* schema_to_string( Schema* a_schema )
{
	switch( a_schema->type )
	{
		case TS_CHAR:
			return "c";

		case TS_INT:
			return "i";

		case TS_REAL:
			return "r";

		case TS_STRING:
			return "s";

		case TS_BOOL:
			return "b";

		case TS_STRUCT:
		{
			char* result = calloc( 2, sizeof( char ) );
			result[ 0 ] = '(';

			Schema* current_attr;
			for( current_attr = a_schema->child;
				 current_attr != NULL;
				 current_attr = current_attr->brother )
			{
				char* schema = schema_to_string( current_attr->child );

				int current_length = strlen( result );
				result = realloc( result, ( strlen( result ) + strlen( current_attr->id ) + strlen( schema ) ) * sizeof( char ) );
				result[ current_length ] = '\0';

				sprintf( result, "%s%s:%s,", result, current_attr->id, schema );
			}
			result[ strlen( result )-1 ] = ')';
			return result;
		}

		case TS_VECTOR:
		{
			char* schema = schema_to_string( a_schema->child );
			char* result = malloc( ( MAX_INT_LEN + strlen( schema ) + 3 * sizeof( char ) ) );
			sprintf( result, "[%d,%s]", a_schema->size, schema );
			return result;
		}

		case TS_ATTR:
			return schema_to_string( a_schema->child );

		default:
			return NULL;
	}
}

/**
 * @brief Function that prints the given code in the output file received as first parameter.
 *
 * @param output A FILE* object that represents the output file where the code will be written.
 * @param code The code to print.
 */
void output_code( FILE* output, Code code )
{
	code = append_code( make_code_one_param( SOL_SCODE, code.size ), code );

	Stat* current_stat;
	for( current_stat = code.head; current_stat != NULL; current_stat = current_stat->next )
		switch( current_stat->op )
		{
			case SOL_FREAD:
			case SOL_READ:
				fprintf( output, "%s %d %d \"%s\"\n",
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].i_val,
						current_stat->args[ 1 ].i_val,
						current_stat->args[ 2 ].s_val );
				break;

			case SOL_LDC:
				fprintf( output, "%s '%s'\n",
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].s_val );
				break;

			case SOL_FRD:
			case SOL_FWR:
			case SOL_FWRITE:
			case SOL_LDS:
			case SOL_RD:
			case SOL_WR:
			case SOL_WRITE:
				fprintf( output, "%s \"%s\"\n",
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].s_val );
				break;

			case SOL_LDR:
				fprintf( output, "%s %f\n",
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].r_val );
				break;

			case SOL_CAT:
			case SOL_LDA:
			case SOL_LOD:
			case SOL_PUSH:
			case SOL_STO:
				fprintf( output, "%s %d %d\n",
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].i_val,
						current_stat->args[ 1 ].i_val );
				break;

			case SOL_EQU:
			case SOL_IGE:
			case SOL_IGT:
			case SOL_IPLUS:
			case SOL_IST:
			case SOL_ITIMES:
			case SOL_IUMI:
			case SOL_NEG:
			case SOL_NEQ:
			case SOL_POP:
			case SOL_RETURN:
			case SOL_RPLUS:
			case SOL_RTIMES:
			case SOL_TOINT:
			case SOL_TOREAL:
			case SOL_HALT:
				fprintf( output, "%s\n",
						CODE_OPERATORS[ current_stat->op ] );
				break;

			default:
				fprintf( output, "%s %d\n",
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].i_val );
				break;
		}
}

