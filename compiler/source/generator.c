#include "generator.h"

extern FILE* yyin;
extern Node* root;
extern Symbol* symbol_table;
extern stacklist scope;

FILE* yyin;

int yygen( FILE* input, FILE* output )
{
	yyin = input;

	int sem_result = yysem();
	if( sem_result != 0 )
		return sem_result;

	stacklist_push( &scope, (stacklist_t) symbol_table );
	Code result = generate_code( root );

	// TODO Replace with something like `output_code'
	code_print( result );

	return 0;
}

Code generate_code( Node* node )
{
	fprintf( stderr, "Working on: %d %d\n", node->type, node->value.i_val );

	Code result = empty_code();

	switch( node->type )
	{
		case T_INT_CONST:
			result = make_ldi( node->value.i_val );
			break;

		case T_CHAR_CONST:
			result = make_ldc( node->value.c_val );
			break;

		case T_REAL_CONST:
			result = make_ldr( node->value.r_val );
			break;

		case T_STR_CONST:
			result = make_lds( node->value.s_val );
			break;

		case T_BOOL_CONST:
			result = make_ldc( node->value.b_val == TRUE ? '1' : '0' );
			break;
			
		case T_ID:
		{
			Symbol* referenced_id = fetch_scope( node->value.s_val );
			result = make_code_two_param( SOL_LOD, referenced_id->nesting, referenced_id->oid );
			break;
		}

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
					result = append_code( result, make_code_one_param( SOL_LDC, 0 ) );
					// The result is now on top of the stack (hopefully, if Lamperti is right)
					break;

				case Q_OR:
					// Expr1 evaluation
					result = append_code( result, expr1 );
					// If expr1 is false, jump to expr2 evaluation
					result = append_code( result, make_code_one_param( SOL_JMF, 3 ) );
					// Else load true and jump after expr2 evaluation
					result = append_code( result, make_code_one_param( SOL_LDC, 1 ) );
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
					}
					break;

				case Q_IN:
					use_me = SOL_IN;
					break;
						
				default:
					break;
			}

			// Pretty standard, really
			result = concatenate_code( result, expr1, expr2, make_code_no_param( use_me ) );

			break;
		}

		case T_MATH_EXPR:
		{
			printf( "CASE: MATH_EXPR(%d)\n", node->value.q_val );

			Node* current_child = node->child;
			
			// Similar to REL_EXPR, but with less cases
			Code expr1 = generate_code( current_child );
			Code expr2 = generate_code( current_child = current_child->brother );

			Schema* operands_schema = infere_expression_schema( current_child );
			Operator use_me;

			printf( "OPERAND TYPE: (%d)\n", operands_schema->type );

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
					}
					break;

				default:
					break;
			}
			
			printf( "RESULTING OPERATOR: (%d)\n\n", use_me );

			// Pretty standard, really
			result = concatenate_code( result, expr1, expr2, make_code_no_param( use_me ) );
			
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
					break;
			}

			// Pretty standard, really
			result = concatenate_code( result, expr, make_code_no_param( use_me ) );
			
			break;
		}

		case N_DYNAMIC_INPUT:
		{
			Node* child = node->child;

			Operator use_me = SOL_RD;

			// Determine if the WR has a specifier-opt or not
			if( child->brother != NULL )
			{
				Code specifier_opt = generate_code( child );

				result = append_code( result, specifier_opt );

				child = child->brother;

				use_me = SOL_FRD;
			}
			
			result = append_code( result, make_code_no_param( use_me ) );
			
			// Format
			result.tail->args[1].s_val = schema_to_string( infere_expression_schema( child ) );

			break;
		}

		case N_DYNAMIC_OUTPUT:
		{
			Node* expr_child = node->child;

			Operator use_me = SOL_WR;

			// Determine if the WR has a specifier-opt or not
			if( expr_child->brother != NULL )
			{
				Code specifier_opt = generate_code( expr_child );

				result = append_code( result, specifier_opt );

				expr_child = expr_child->brother;

				use_me = SOL_FWR;
			}
			
			Schema* expression_schema = infere_expression_schema( expr_child );
			Code expr = generate_code( expr_child );

			result = concatenate_code( result, expr, make_code_no_param( use_me ) );
			
			result.tail->args[1].s_val = schema_to_string( expression_schema );

			break;
		}
	
		case T_UNQUALIFIED_NONTERMINAL:
			switch( node->value.n_val )
			{
				case N_FUNC_DECL:
				{
					Node* current_node = node->child;
					// TODO Find a way to avoid to duplicate the base function.
					stacklist_push( &scope, (stacklist_t) fetch_scope( current_node->value.s_val ) );
					current_node = current_node->brother;
					if( current_node->type == T_UNQUALIFIED_NONTERMINAL )
					{
						// TODO Params?
						current_node = current_node->brother;
					}
					current_node = current_node->brother;

					if( current_node->value.n_val == N_TYPE_SECT )
						current_node = current_node->brother;
					if( current_node->value.n_val == N_VAR_SECT )
					{
						result = append_code( result, generate_code( current_node ) );
						current_node = current_node->brother;
						}
						if( current_node->value.n_val == N_CONST_SECT )
						{
							result = append_code( result, generate_code( current_node ) );
							current_node = current_node->brother;
						}
						if( current_node->value.n_val == N_FUNC_LIST )
						{
							Node* current_child = current_node->child;
							do
							{
								result = append_code( result, generate_code( current_child ) );
								current_child = current_child->brother;
							} while( current_child != NULL );

							current_node = current_node->brother;
						}

						// FIXME Work on every statement
						result = append_code( result, generate_code( current_node->child->brother ) );
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
							Node* current_id = current_child->child;
							while( current_id->brother->brother != NULL )
							{
								result = append_code( result, make_decl( fetch_scope( current_id->value.s_val )->schema ) );
								const_size++;

								current_id = current_id->brother;
							}

							for( ; const_size > 0; const_size-- )
								values = append_code( values, generate_code( current_id->brother ) );

							current_child = current_child->brother;
						}

						result = append_code( result, values );
						break;
					}
					
					case N_FIELDING:
					{
						Node* current_node = node->child;
						// Something like
						Symbol* struct_table = fetch_scope( current_node->value.s_val );
						result = make_code_two_param
								 (
									 SOL_LDA, 
									 struct_table->nesting,
									 struct_table->oid
								 );
						
						// Reaching the requested filed address in the struct
						Code load_field = empty_code();
						Schema* field_schema = struct_table->schema;
						current_node = current_node->brother;
						while( field_schema != NULL && field_schema->id != current_node->value.s_val )
						{
							result = append_code
									 (
										 result,
										 make_code_one_param
										 (
											 SOL_FDA,
											 schema_size( field_schema )
										 )
									 );
											
							field_schema = field_schema->brother;
						}

						// FIXME
						if( field_schema == NULL )
							break;
						
						// I hope it works like this
						result = append_code( result,
											  make_code_one_param
											  (
												  // FIXME
												  ( field_schema->size == 1 ? SOL_EIL : SOL_SIL ),
												  field_schema->size * schema_size( field_schema )
											  )
						);
						break;
					}

					case N_INDEXING:
					{
						Node* current_node = node->child;
						
						Symbol* array_table = fetch_scope( current_node->value.s_val );
						result = make_code_two_param
											(
												SOL_LDA, 
												array_table->nesting,
												array_table->oid
											);
						
						// Computing index value
						current_node = current_node->brother;
						result = append_code( result, generate_code( current_node ) );
				
						// Going deeply until i don't reach the final type of the array
						Code ixa_code = empty_code();
						Schema* index_schema = array_table->schema;
						while( index_schema->child != NULL )
						{
							index_schema = index_schema->child;
							result = append_code
									 (
										 result,
										 make_code_one_param
										 (
											 SOL_IXA,
											 schema_size( index_schema )
										 )
									 );
						}
						
						result = append_code( result,
											  make_code_one_param
											  (
												  SOL_EIL,
												  schema_size( index_schema )
											  )
											);
						
						break;
					}

					case N_COND_EXPR:
					{
						Node* current_child = node->child;
						Code condition = generate_code( current_child );
						Code value = generate_code( current_child = current_child->brother );
						Code next = make_code_one_param( SOL_JMF, value.size + 2 );
						Code exit = make_code_no_param( SOL_JMP );
						
						result = concatenate_code( condition, next, value, exit );
						Code exit_list = exit;
						
						while( current_child->value.n_val == N_ELSIF_EXPR )
						{
							condition = generate_code( current_child->child );
							value = generate_code( current_child = current_child->child->brother );
							next = make_code_one_param( SOL_JMF, value.size + 2 );
							exit = make_code_no_param( SOL_JMP );
							
							result = concatenate_code( result, condition, next, value, exit );
							exit_list = append_code( exit_list, exit );
							
							current_child = current_child->brother;
						}
						
						result = append_code( result, generate_code( current_child ) );
						
						Stat* exit_stat;
						for( exit_stat = exit_list.head; exit_stat != NULL; exit_stat = exit_stat->next )
							exit_stat->args[ 0 ].i_val = result.size - exit_stat->address + 1;
						
						break;
					}
					
					case N_READ_STAT:
					{
						Symbol* referenced_id;
						if( node->child->brother == NULL )
						{
							referenced_id = fetch_scope( node->child->value.s_val );
							result = make_code_two_param( SOL_READ, referenced_id->nesting, referenced_id->oid );
						}
						else
						{
							referenced_id = fetch_scope( node->child->brother->value.s_val );
							result = append_code( generate_code( node->child ),
												  make_code_two_param( SOL_READ, referenced_id->nesting, referenced_id->oid ) );
						}
							
						result.tail->args[ 2 ].s_val = schema_to_string( referenced_id->schema );
							
						break;
					}

					default:

						// FIXME Debug only
						if( node->child != NULL )
							result = append_code( result, generate_code( node->child ) );
						if( node->brother != NULL )
							result = append_code( result, generate_code( node->brother ) );
						break;
				}
				break;

		default:

			// FIXME Debug only
			if( node->child != NULL )
				result = append_code( result, generate_code( node->child ) );
			if( node->brother != NULL )
				result = append_code( result, generate_code( node->brother ) );
			break;
	}

	return result;
}

Code empty_code( void )
{
	static Code code = { NULL, 0, NULL };
	return code;
}

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

Code append_code( Code first, Code second )
{
	if( first.size == 0 )
		return second;
	if( second.size == 0 )
		return first;

	Code result;
	relocate_address( second, first.size );
	result.head = first.head;
	result.tail = second.tail;
	first.tail->next = second.head;
	result.size = first.size + second.size;

	return result;
}

Code concatenate_code( Code code1, Code code2, ... )
{
	Code result = code1;

	Code* current_code;
	for( current_code = &code2; current_code != NULL; current_code++ )
		result = append_code( result, *current_code );

	return result;
}

Stat* new_stat( Operator op )
{
	Stat* result = malloc( sizeof( Stat ) );
	result->address = 0;
	result->op = op;
	result->next = NULL;

	return result;
}

Code make_code_no_param( Operator op )
{
	Code result;
	result.head = result.tail = new_stat(op);
	result.size = 1;

	return result;
}

Code make_code_one_param( Operator op, int arg )
{
	Code result = make_code_no_param(op);
	result.head->args[ 0 ].i_val = arg;

	return result;
}

Code make_code_two_param( Operator op, int arg1, int arg2 )
{
	Code result = make_code_no_param(op);
	result.head->args[ 0 ].i_val = arg1;
	result.head->args[ 1 ].i_val = arg2;

	return result;
}

Code make_push_pop( int size, int chain, int entry )
{
	return concatenate_code( make_code_two_param( SOL_PUSH, size, chain ),
							 make_code_one_param( SOL_GOTO, entry ),
							 make_code_no_param( SOL_POP ) );
}

Code make_ldc( char a_char )
{
	Code result;
	result = make_code_no_param( SOL_LDC );
	result.head->args[ 0 ].c_val = a_char;

	return result;
}

Code make_ldi( int an_int )
{
	Code result;
	result = make_code_no_param( SOL_LDI );
	result.head->args[ 0 ].i_val = an_int;

	return result;
}

Code make_ldr( float a_real )
{
	Code result;
	result = make_code_no_param( SOL_LDR );
	result.head->args[ 0 ].r_val = a_real;

	return result;
}

Code make_lds( char* a_string )
{
	Code result;
	result = make_code_no_param( SOL_LDS );
	result.head->args[ 0 ].s_val = a_string;

	return result;
}

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
	}
}

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
			char* result = malloc( 2 * sizeof( char ) );
			result[ 0 ] = '(';

			Schema* current_attr;
			for( current_attr = a_schema->child;
				 current_attr != NULL;
				 current_attr = current_attr->brother )
			{
				char* schema = schema_to_string( current_attr->child );
				result = realloc( result, ( strlen( result ) + strlen( current_attr->id ) + strlen( schema ) ) * sizeof( char ) );
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
	}
}

/**
 * @brief Error function called in case of error while generating code.
 *
 * @param node
 * @param type
 *
 * @return
 */
int yygenerror( Node* node, char* type )
{
	return 0;
}
