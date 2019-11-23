%{
#include "mast.hpp"

Ast *yyroot;
extern int yylex();
extern void yyerror(const char *s);
%}

%union {
  char  c;
  float f;
  Ast  *a;
}
%token BR NUMBER OPERATOR
%type<a> e
%type<c> op
%start p

%%
p:
    e BR  { yyroot = $1; }
;
e:
    '(' op e e ')'  { $$ = new Exp($2, $3, $4); }
  | NUMBER          { $$ = new Num(yylval.f); }
;
op:
    OPERATOR  { $$ = yylval.c; }
;

%%
