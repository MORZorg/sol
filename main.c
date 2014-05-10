#include <stdio.h>

#ifdef PARSER

extern FILE *yyin;
extern int yyparse();

int main()
{
  yyin = stdin;

  // TODO
  /* if ((result = yyparse()) == 0) */
  /*   treeprint(root, 0); */

  return yyparse();
}

#else
#ifdef LEXER

#include "parser.h"

extern int yylex();
extern char* yytext;

int main()
{
  do
  {
    yylex();
  } while (1);

  return 0;
}

#else

#include <stdlib.h>

int main()
{
  fprintf(stderr, "No body supplied. Exiting.");
  exit(1);
}

#endif
#endif
