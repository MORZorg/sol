%{
#include <string.h>

#include "def.h"

#define YYSTYPE Node*
extern FILE* yyin;
extern char* yytext;
extern Value lexval;
extern int line;

extern int yylex();
extern int yyerror( char* );

Node* root = NULL;
%}

%token FUNC CHAR INT REAL STRING BOOL STRUCT VECTOR OF TYPE VAR CONST
%token SOL_BEGIN END IF THEN ENDIF ELSIF ELSE WHILE DO ENDWHILE FOR TO ENDFOR FOREACH ENDFOREACH
%token RETURN READ WRITE RD WR DEFINE ASSIGN BREAK
%token AND OR IN NOT TOINT TOREAL EQ NEQ GT GEQ LT LEQ PLUS MINUS MULTIPLY DIVIDE ID
%token INT_CONST CHAR_CONST REAL_CONST STR_CONST BOOL_CONST

%%

program : func_decl { root = $1; }
        ;

func_decl : FUNC ID { $$ = new_terminal_node( T_ID, lexval ); }
            '(' par_list ')' DEFINE domain
            type_sect_opt var_sect_opt const_sect_opt func_list_opt func_body
            {
                $$ = new_nonterminal_node( N_FUNC_DECL );
                $$->child = $3;
                Node** current = &($$->child->brother);
                current = assign_brother( current, $5 );
                current = assign_brother( current, $8 );
                current = assign_brother( current, $9 );
                current = assign_brother( current, $10 );
                current = assign_brother( current, $11 );
                current = assign_brother( current, $12 );
                current = assign_brother( current, $13 );
            }
          ;

par_list : decl_list { $$ = new_nonterminal_node( N_PAR_LIST ); $$->child = $1; }
         | { $$ = NULL; }
         ;

decl_list : decl ';' decl_list { $$ = new_nonterminal_node( N_DECL ); $$->child = $1; $$->brother = $3; }
          | decl ';' { $$ = new_nonterminal_node( N_DECL ); $$->child = $1; }
          ;

decl : id_list DEFINE domain {
                                  $$ = $1;
								  assign_brother( &($1->brother), $3 );
                             }
     ;

id_list : ID { $$ = new_terminal_node( T_ID, lexval ); } ',' id_list { $$ = $2; $2->brother = $4; }
        | ID { $$ = new_terminal_node( T_ID, lexval ); }
        ;

domain : atomic_domain
       | struct_domain { $$ = new_nonterminal_node( N_STRUCT_DOMAIN ); $$->child = $1; }
       | vector_domain { $$ = new_nonterminal_node( N_VECTOR_DOMAIN ); $$->child = $1; }
       | ID			   { $$ = new_terminal_node( T_ID_DOMAIN, lexval ); }
       ;

atomic_domain : CHAR	{ $$ = new_qualified_node( T_ATOMIC_DOMAIN, Q_CHAR ); }
              | INT		{ $$ = new_qualified_node( T_ATOMIC_DOMAIN, Q_INT ); }
              | REAL	{ $$ = new_qualified_node( T_ATOMIC_DOMAIN, Q_REAL ); }
              | STRING	{ $$ = new_qualified_node( T_ATOMIC_DOMAIN, Q_STRING ); }
              | BOOL	{ $$ = new_qualified_node( T_ATOMIC_DOMAIN, Q_BOOL ); }
              ;

struct_domain : STRUCT '(' decl_list ')' { $$ = $3; }
              ;

vector_domain : VECTOR '[' INT_CONST { $$ = new_terminal_node( T_INT_CONST, lexval ); } ']' OF domain { $$ = $4; $4->brother = $7; }
              ;

type_sect_opt : TYPE decl_list { $$ = new_nonterminal_node( N_TYPE_SECT ); $$->child = $2; }
              | { $$ = NULL; }
              ;

var_sect_opt : VAR decl_list { $$ = new_nonterminal_node( N_VAR_SECT ); $$->child = $2; }
             | { $$ = NULL; }
             ;

const_sect_opt : CONST const_list { $$ = new_nonterminal_node( N_CONST_SECT ); $$->child = $2; }
              | { $$ = NULL; }
              ;

const_list : const_decl const_list { $$ = new_nonterminal_node( N_CONST_DECL ); $$->child = $1; $$->brother = $2; }
           | const_decl { $$ = new_nonterminal_node( N_CONST_DECL ); $$->child = $1; }
           ;

const_decl : decl ASSIGN expr ';' { $$ = $1; assign_brother( &($$->brother), $3 ); }
           ;

func_list_opt : func_list { $$ = new_nonterminal_node( N_FUNC_LIST ); $$->child = $1; }
              | { $$ = NULL; }
              ;

func_list : func_decl func_list { $$ = $1; $$->brother = $2; }
          | func_decl { $$ = $1; }
          ;

func_body : SOL_BEGIN ID { $$ = new_terminal_node( T_ID, lexval ); } stat_list END ID { $$ = new_terminal_node( T_ID, lexval ); }
			{ 
                if( $3->value.s_val != $7->value.s_val )
                    yyerror( "unmatched identifier" );

				$$ = new_nonterminal_node( N_FUNC_BODY );
				$$->child = $3;
				$$->child->brother = $4;
			}
          ;

stat_list : stat ';' stat_list
		    {
				$$ = new_nonterminal_node( N_STAT_LIST );
				$$->child = $1;
				$$->child->brother = $3->child;
			}
          | stat ';'
			{
				$$ = new_nonterminal_node( N_STAT_LIST );
				$$->child = $1;
			}
          ;

stat : assign_stat { $$ = $1; }
     | if_stat { $$ = $1; }
     | while_stat { $$ = $1; }
     | for_stat { $$ = $1; }
     | foreach_stat { $$ = $1; }
     | return_stat { $$ = $1; }
     | read_stat { $$ = $1; }
     | write_stat { $$ = $1; }
     | BREAK { $$ = new_terminal_node( T_BREAK_STAT, lexval ); }
     ;

assign_stat : left_hand_side ASSIGN expr
			  {
				$$ = new_nonterminal_node( N_ASSIGN_STAT );
				$$->child = $1;
				$$->child->brother = $3;
			  }
            ;

left_hand_side : ID { $$ = new_terminal_node( T_ID, lexval ); }
               | fielding { $$ = $1; }
               | indexing { $$ = $1; }
               ;

fielding : left_hand_side '.' ID { $$ = new_terminal_node( T_ID, lexval ); }
		   {
				$$ = new_nonterminal_node( N_FIELDING );
				$$->child = $1;
				$$->child->brother = $4;
		   }
         ;

indexing : left_hand_side '[' expr ']'
		   {
				$$ = new_nonterminal_node( N_INDEXING );
				$$->child = $1;
				$$->child->brother = $3;
		   }
         ;

if_stat : IF expr THEN stat_list elsif_stat_list_opt else_stat_opt ENDIF
		  {
				$$ = new_nonterminal_node( N_IF_STAT );
				$$->child = $2;
				Node** current = &( $$->child->brother );
				current = assign_brother( current, $4 );
				current = assign_brother( current, $5 );
				current = assign_brother( current, $6 );
		  }
        ;

elsif_stat_list_opt : ELSIF expr THEN stat_list elsif_stat_list_opt
					  {
							$$ = new_nonterminal_node( N_ELSIF_STAT );
							$$->child = $2;
							Node** current = &( $$->child->brother );
							current = assign_brother( current, $4 );
							current = assign_brother( current, $5 );
					  }
                    | { $$ = NULL; }
                    ;

else_stat_opt : ELSE stat_list { $$ = new_nonterminal_node( N_ELSE_STAT ); $$->child = $2; }
              | { $$ = NULL; }
              ;

while_stat : WHILE expr DO stat_list ENDWHILE
		     {
				$$ = new_nonterminal_node( N_WHILE_STAT );
				$$->child = $2;
				$$->child->brother = $4;
			 }
           ;

for_stat : FOR ID { $$ = new_terminal_node( T_ID, lexval ); } ASSIGN expr TO expr DO stat_list ENDFOR
		   {
				$$ = new_nonterminal_node( N_FOR_STAT );
				$$->child = $3;
				Node** current = &( $$->child->brother );
				current = assign_brother( current, $5 );
				current = assign_brother( current, $7 );
				current = assign_brother( current, $9 );
		   }
         ;

foreach_stat : FOREACH ID { $$ = new_terminal_node( T_ID, lexval ); } IN expr DO stat_list ENDFOREACH
			   {
					$$ = new_nonterminal_node( N_FOREACH_STAT );
					$$->child = $3;
					Node** current = &( $$->child->brother );
					current = assign_brother( current, $5 );
					current = assign_brother( current, $7 );
			   }
             ;

return_stat : RETURN expr
			  {
				$$ = new_nonterminal_node( N_RETURN_STAT );
				$$->child = $2;
			  }
            ;

read_stat : READ specifier_opt ID { $$ = new_terminal_node( T_ID, lexval ); }
		    {
				$$ = new_nonterminal_node( N_READ_STAT );
				$$->child = $2;
				$$->child->brother = $4;
			}
          ;

specifier_opt : '[' expr ']' { $$ = $2; }
              | { $$ = NULL; }
              ;

write_stat : WRITE specifier_opt expr
		     {
				$$ = new_nonterminal_node( N_WRITE_STAT );

                if( $2 == NULL )
                    $$->child = $3;
                else
                {
                    $$->child = $2;
                    $$->child->brother = $3;
                }
			 }
           ;

expr : expr bool_op bool_term
	   {
			$$ = $2;
			$$->child = $1;
			$1->brother = $3;
       }
     | bool_term
	   {
			if( $1->brother != NULL )
			{
				$$ = new_nonterminal_node( N_EXPR );
				$$->child = $1;
			}
			else
				$$ = $1;
	   }
     ;

bool_op : AND { $$ = new_qualified_node( T_LOGIC_EXPR, Q_AND ); }
        | OR { $$ = new_qualified_node( T_LOGIC_EXPR, Q_OR ); }
        ;

bool_term : rel_term rel_op rel_term
			{
				$$ = $2;
				$$->child = $1;
				$1->brother = $3;
			}
          | rel_term
			{
				if( $1->brother != NULL )
				{
					$$ = new_nonterminal_node( N_BOOL_TERM );
					$$->child = $1;
				}
				else
					$$ = $1;
			}
          ;

rel_op : EQ   { $$ = new_qualified_node( T_REL_EXPR, Q_EQ  ); }
       | NEQ  { $$ = new_qualified_node( T_REL_EXPR, Q_NEQ ); }
       | GT   { $$ = new_qualified_node( T_REL_EXPR, Q_GT  ); }
       | GEQ  { $$ = new_qualified_node( T_REL_EXPR, Q_GEQ ); }
       | LT   { $$ = new_qualified_node( T_REL_EXPR, Q_LT  ); }
       | LEQ  { $$ = new_qualified_node( T_REL_EXPR, Q_LEQ ); }
       | IN   { $$ = new_qualified_node( T_REL_EXPR, Q_IN  ); }
       ;

rel_term : rel_term low_bin_op low_term
		   {
				$$ = $2;
				$$->child = $1;
				$1->brother = $3;
		   }
         | low_term
		   {
				if( $1->brother != NULL )
				{
					$$ = new_nonterminal_node( N_REL_TERM );
					$$->child = $1;
				}
				else
					$$ = $1;
		   }
         ;

low_bin_op : PLUS { $$ = new_qualified_node( T_MATH_EXPR, Q_PLUS ); }
           | MINUS { $$ = new_qualified_node( T_MATH_EXPR, Q_MINUS ); }
           ;

low_term : low_term high_bin_op factor 
		   {
				$$ = $2;
				$$->child = $1;
				$1->brother = $3;
		   }
         | factor
		   {
				if( $1->brother != NULL )
				{
					$$ = new_nonterminal_node( N_LOW_TERM );
					$$->child = $1;
				}
				else
					$$ = $1;
		   }
         ;

high_bin_op : MULTIPLY { $$ = new_qualified_node( T_MATH_EXPR, Q_MULTIPLY ); }
            | DIVIDE { $$ = new_qualified_node( T_MATH_EXPR, Q_DIVIDE ); }
            ;

factor : unary_op factor { $$ = $1; assign_brother( &($$->child), $2 ); }
       | '(' expr ')' { $$ = $2; }
       | left_hand_side { $$ = $1; }
       | atomic_const { $$ = $1; }
       | instance_construction { $$ = $1; }
       | func_call { $$ = $1; }
       | cond_expr { $$ = $1; }
       | built_in_call { $$ = $1; }
       | dynamic_input { $$ = $1; }
       ;

unary_op : MINUS { $$ = new_qualified_node( T_NEG_EXPR, Q_MINUS ); }
         | NOT { $$ = new_qualified_node( T_NEG_EXPR, Q_NOT ); } 
         | dynamic_output { $$ = $1; }
         ;

atomic_const : CHAR_CONST { $$ = new_terminal_node( T_CHAR_CONST, lexval ); }
             | INT_CONST { $$ = new_terminal_node( T_INT_CONST, lexval ); }
             | REAL_CONST { $$ = new_terminal_node( T_REAL_CONST, lexval ); }
             | STR_CONST { $$ = new_terminal_node( T_STR_CONST, lexval ); }
             | BOOL_CONST { $$ = new_terminal_node( T_BOOL_CONST, lexval ); }
             ;

instance_construction : struct_construction { $$ = $1; }
                      | vector_construction { $$ = $1; }
                      ;

struct_construction : STRUCT '(' expr_list ')'
					  {
                          $$ = new_qualified_node( T_INSTANCE_EXPR, Q_STRUCT );
                          $$->child = $3;
					  }
                    ;

expr_list : expr ',' expr_list { $$ = $1; $$->brother = $3; }
          | expr { $$ = $1; }
          ;

vector_construction : VECTOR '(' expr_list ')'
					  {
                          $$ = new_qualified_node( T_INSTANCE_EXPR, Q_VECTOR );
                          $$->child = $3;
					  }
                    ;

func_call : ID { $$ = new_terminal_node( T_ID, lexval ); } '(' expr_list_opt ')'
			{
				$$ = new_nonterminal_node( N_FUNC_CALL );
				$$->child = $2;
				$$->child->brother = $4;
			}
          ;

expr_list_opt : expr_list { $$ = $1; }
              | { $$ = NULL; }
              ;

cond_expr : IF expr THEN expr elsif_expr_list_opt ELSE expr ENDIF
			{
				$$ = new_nonterminal_node( N_COND_EXPR );
				$$->child = $2;
				Node **current = &( $$->child->brother );
				current = assign_brother( current, $4 );
				current = assign_brother( current, $5 );
				current = assign_brother( current, $7 );
			}
          ;

elsif_expr_list_opt : ELSIF expr THEN expr elsif_expr_list_opt
					  {
						$$ = new_nonterminal_node( N_ELSIF_EXPR );
						$$->child = $2;
						Node **current = &( $$->child->brother );
						current = assign_brother( current, $4 );
						/* assign_brother( current, $5 ); */
						$$->brother = $5;
					  }
                    | { $$ = NULL; }
                    ;

built_in_call : toint_call { $$ = $1; }
              | toreal_call { $$ = $1; }
              ;

toint_call : TOINT '(' expr ')'
			 {
				$$ = new_qualified_node( T_BUILT_IN_CALL, Q_TOINT );
				$$->child = $3; 
			 }
           ;

toreal_call : TOREAL '(' expr ')'
			  {
				$$ = new_qualified_node( T_BUILT_IN_CALL, Q_TOREAL );
				$$->child = $3;
			  }
            ;

dynamic_input : RD specifier_opt domain
				{
					$$ = new_nonterminal_node( N_DYNAMIC_INPUT );

                    if( $2 == NULL )
                        $$->child = $3;
                    else
                    {
                        $$->child = $2;
                        $2->brother = $3;
                    }
				}
              ;

dynamic_output : WR specifier_opt
				 {
					$$ = new_nonterminal_node( N_DYNAMIC_OUTPUT );

					$$->child = $2;
				}
               ;

%%

Node* new_node( TypeNode type )
{
	Node* result = malloc( sizeof( Node ) );
	result->type = type;
	result->child = result->brother = NULL;
	result->line = line;

	return result;
}

Node* new_nonterminal_node( Nonterminal value )
{
	Node* result = new_node( T_UNQUALIFIED_NONTERMINAL );
	result->value.n_val = value;

	return result;
}

Node* new_terminal_node( TypeNode type, Value value )
{
	Node* result = new_node( type );
	
	switch( type )
	{
		case T_INT_CONST:
			result->value.i_val = value.i_val;
			break;

        case T_CHAR_CONST:
            result->value.c_val = value.c_val;
            break;

        case T_REAL_CONST:
            result->value.r_val = value.r_val;
            break;

        case T_BOOL_CONST:
            result->value.b_val = value.b_val;
            break;

        case T_STR_CONST:
		case T_ID:
        case T_ID_DOMAIN:
			result->value.s_val = value.s_val;
			break;

		default:
			break;
	}

	return result;
}

Node* new_qualified_node( TypeNode type, Qualifier qualifier )
{
	Node* result = new_node( type );
	result->value.q_val = qualifier;

	return result;
}

/**
 * @brief Assigns to the given pointer to a tree node (Node*) the given tree
 *        node or its last brother.
 * @note Can be optimized if brother can't be NULL.
 *
 * @param initial
 * @param brother
 *
 * @return A pointer to the given tree node's brother in case of success,
 *	the given pointer to a tree node otherwise.
 */
Node** assign_brother( Node** initial, Node* brother )
{
	while( *initial != NULL )
	{
		initial = &( (*initial)->brother );
	}

	if( brother != NULL )
	{
		*initial = brother;
		return &( brother->brother );
	}

	return initial;
}

Node* get_last_brother( Node* node )
{
    Node* current_node = node;
    while( current_node->brother != NULL )
        current_node = current_node->brother;

    return current_node;
}

void print_node( Node* node )
{
	fprintf( stderr, "\n *** NODE *** \t" );
	fprintf( stderr, "Line: %d\t", node->line );
	fprintf( stderr, "TypeNode: %d\t\n", node->type );
}
