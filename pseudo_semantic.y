//
%%

program : func_decl 
        ;

func_decl : FUNC ID (' decl_list_opt ')' DEFINE domain type_sect_opt var_sect_opt const_sect_opt func_list_opt func_body
            
          ;

decl_list_opt : decl_list 
              | 
              ;

decl_list : decl ';' decl_list 
          | decl ';' 
          ;

decl : id_list DEFINE domain 
        {
          foreach id in id_list do 
            insert_in_table( id.name, domain )
        }
     ;

id_list : ID  ',' id_list 
        | ID 
        ;

domain : atomic_domain  { this.domain = domain; }
       | struct_domain  { this.domain = domain; }
       | vector_domain  { this.domain = domain; }
       | ID			    { this.domain = domain; }
       ;

atomic_domain : CHAR	{ domain = char; }
              | INT		{ domain = int; }
              | REAL	{ domain = real; }
              | STRING	{ domain = string; }
              | BOOL	{ domain = bool; }
              ;

struct_domain : STRUCT '(' decl_list ')' 
                {
                  this.domain = struct_type;
                  this.domain->child = build_type_tree( decl_list );
                }
              ;

vector_domain : VECTOR '[' INT_CONST  ']' OF domain 
                {
                  this.domain = vector_type( int_const ];
                  this.domain->child = domain;
                }
              ;

type_sect_opt : TYPE decl_list 
              | 
              ;

var_sect_opt : VAR decl_list 
             | 
             ;

const_sect_opt : CONST const_list 
              | 
              ;

const_list : const_decl const_list 
           | const_decl 
           ;

const_decl : decl ASSIGN expr ';' 
           ;

func_list_opt : func_list 
              | 
              ;

func_list : func_decl func_list 
          | func_decl 
          ;

func_body : SOL_BEGIN ID  stat_list END ID 
			
          ;

stat_list : stat ';' stat_list
		    
          | stat ';' 
          ;

stat : assign_stat 
     | if_stat 
     | while_stat 
     | for_stat 
     | foreach_stat 
     | return_stat 
     | read_stat 
     | write_stat 
     ;

assign_stat : left_hand_side ASSIGN expr
			  
            ;

left_hand_side : ID 
               | fielding 
               | indexing 
               ;

fielding : left_hand_side '.' ID 
		   
         ;

indexing : left_hand_side '[' expr ']'
		   
         ;

if_stat : IF expr THEN stat_list elsif_stat_list_opt else_stat_opt ENDIF
		  
        ;

elsif_stat_list_opt : ELSIF expr THEN stat_list elsif_stat_list_opt
					  
                    | 
                    ;

else_stat_opt : ELSE stat_list 
              | 
              ;

while_stat : WHILE expr DO stat_list ENDWHILE
		     
           ;

for_stat : FOR ID  ASSIGN expr TO expr DO stat_list ENDFOR
		   
         ;

foreach_stat : FOREACH ID  IN expr DO stat_list ENDFOREACH
			   
             ;

return_stat : RETURN expr
			  
            ;

read_stat : READ specifier_opt ID 
		    
          ;

specifier_opt : '[' expr ']' 
              | 
              ;

write_stat : WRITE specifier_opt expr
		     
			 
           ;

expr : expr bool_op bool_term
	   
     | bool_term
	   
			else
				$$ = $1;
	   
     ;

bool_op : AND 
        | OR 
        ;

bool_term : rel_term rel_op rel_term
			
          | rel_term
			
				else
					$$ = $1;
			
          ;

rel_op : EQ   
       | NEQ  
       | GT   
       | GEQ  
       | LT   
       | LEQ  
       | IN   
       ;

rel_term : rel_term low_bin_op low_term
		   
         | low_term
		   
				else
					$$ = $1;
		   
         ;

low_bin_op : PLUS 
           | MINUS 
           ;

low_term : low_term high_bin_op factor 
		   
         | factor
		   
				else
					$$ = $1;
		   
         ;

high_bin_op : MULTIPLY 
            | DIVIDE 
            ;

factor : unary_op factor 
       | '(' expr ')' 
       | left_hand_side 
       | atomic_const 
       | instance_construction 
       | func_call 
       | cond_expr 
       | built_in_call 
       | dynamic_input 
       ;

unary_op : MINUS 
         | NOT  
         | dynamic_output 
         ;

atomic_const : CHAR_CONST 
             | INT_CONST 
             | REAL_CONST 
             | STR_CONST 
             | BOOL_CONST 
             ;

instance_construction : struct_construction 
                      | vector_construction 
                      ;

struct_construction : STRUCT '(' expr_list ')'
					  
                    ;

expr_list : expr ',' expr_list 
          | expr 
          ;

vector_construction : VECTOR '(' expr_list ')'
					  
                    ;

func_call : ID  '(' expr_list_opt ')'
			
          ;

expr_list_opt : expr_list 
              | 
              ;

cond_expr : IF expr THEN expr elsif_expr_list_opt ELSE expr ENDIF
			
          ;

elsif_expr_list_opt : ELSIF expr THEN expr elsif_expr_list_opt
					  
                    | 
                    ;

built_in_call : toint_call 
              | toreal_call 
              ;

toint_call : TOINT '(' expr ')'
			 
           ;

toreal_call : TOREAL '(' expr ')'
			  
            ;

dynamic_input : RD specifier_opt domain
				
				
              ;

dynamic_output : WR specifier_opt
				 
               ;

%%
