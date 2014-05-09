%{
#include "def.h"
// TODO
/* #define YYSTYPE Pnode */
extern char *yytext;
extern Value lexval;
extern int line;
extern FILE *yyin;
/* Pnode root = NULL; */
%}

%token FUNC CHAR INT REAL STRING BOOL STRUCT VECTOR OF TYPE VAR CONST
%token SOL_BEGIN END IF THEN ENDIF ELSIF ELSE WHILE DO ENDWHILE FOR TO ENDFOR FOREACH ENDFOREACH
%token RETURN READ WRITE RD WR DEFINE ASSIGN
%token AND OR IN NOT TOINT TOREAL EQ NEQ GT GEQ LT LEQ PLUS MINUS MULTIPLY DIVIDE ID
%token INT_CONST CHAR_CONST REAL_CONST STR_CONST BOOL_CONST
%token ERROR

%%

program : func_decl
        ;

func_decl : FUNC ID '(' decl_list_opt ')' DEFINE domain type_sect_opt var_sect_opt const_sect_opt func_list_opt func_body
          ;

decl_list_opt : decl_list
              |
              ;

decl_list : decl ';' decl_list
          | decl ';'
          ;

decl : id_list ':' domain
     ;

id_list : ID ',' id_list
        | ID
        ;

domain : atomic_domain
       | struct_domain
       | vector_domain
       | ID
       ;

atomic_domain : CHAR
              | INT
              | REAL
              | STRING
              | BOOL
              ;

struct_domain : STRUCT '(' decl_list ')'
              ;

vector_domain : VECTOR '[' INT_CONST ']' OF domain
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

func_body : SOL_BEGIN ID stat_list END ID
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

while_stat : WHILE expr DO  stat_list ENDWHILE
           ;

for_stat : FOR ID ASSIGN expr TO expr DO stat_list ENDFOR
         ;

foreach_stat : FOREACH ID IN expr DO stat_list ENDFOREACH
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
     ;

bool_op : AND
        | OR
        ;

bool_term : rel_term rel_op rel_term
          | rel_term
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
         ;

low_bin_op : PLUS
           | MINUS
           ;

low_term : low_term high_bin_op factor
         | factor
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

func_call : ID '(' expr_list_opt ')'
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

int main()
{
  int result;

  yyin = stdin;
  // TODO
  /* if ((result = yyparse()) == 0) */
  /*   treeprint(root, 0); */
  if (yyparse() == 0)
    printf("Success");

  return result;
}

int yyerror()
{
  fprintf(stderr, "Line %d: syntax error: unexpected symbol \"%s\".\n", line, yytext);
  exit(-1);
}

