%{
#include "mast.hpp"

Ast *yyroot;
extern int yylex();
extern void yyerror(const char *s);
%}

%union {
  bool    b;
  char    c;
  int     i;
  long    l;
  float   f;
  double  d;
  char   *s;
  Ast    *a;
}
%token REM BR
%token BOOLEANV INTEGERV LONGV SINGLEV DOUBLEV STRINGV ID
%left AND OR XOR IMP EQV
%left NOT
%left EQ LT GT LE GE NE
%left MOD
%left ADD SUB
%left MUL DIV IDIV
%left POW
%type<a> e n x
%start p

%%
p:
    e BR  { yyroot = $1; }
;
e: 
    e AND e   { $$ = new Call("and", {$1, $3}); }
  | e OR e    { $$ = new Call("or", {$1, $3}); }
  | e XOR e   { $$ = new Call("xor", {$1, $3}); }
  | e IMP e   { $$ = new Call("imp", {$1, $3}); }
  | e EQV e   { $$ = new Call("eqv", {$1, $3}); }
  | NOT e     { $$ = new Call("not", {$2}); }
  | e EQ e    { $$ = new Call("=", {$1, $3}); }
  | e LT e    { $$ = new Call("<", {$1, $3}); }
  | e GT e    { $$ = new Call(">", {$1, $3}); }
  | e LE e    { $$ = new Call("<=", {$1, $3}); }
  | e GE e    { $$ = new Call(">=", {$1, $3}); }
  | e NE e    { $$ = new Call("<>", {$1, $3}); }
  | e MOD e   { $$ = new Call("mod", {$1, $3}); }
  | e ADD e   { $$ = new Call("+", {$1, $3}); }
  | e SUB e   { $$ = new Call("-", {$1, $3}); }
  | e MUL e   { $$ = new Call("*", {$1, $3}); }
  | e DIV e   { $$ = new Call("/", {$1, $3}); }
  | e IDIV e  { $$ = new Call("\\", {$1, $3}); }
  | e POW e   { $$ = new Call("^", {$1, $3}); }
  | '(' e ')' { $$ = $2; }
  | ADD e     { $$ = new Call("+0", {$2}); }
  | SUB e     { $$ = new Call("-0", {$2}); }
  | n
  | x
;
n:
    BOOLEANV  { $$ = new Litr(yylval.b); }
  | INTEGERV  { $$ = new Litr((long) yylval.i); }
  | LONGV     { $$ = new Litr(yylval.l); }
  | SINGLEV   { $$ = new Litr(yylval.f); }
  | DOUBLEV   { $$ = new Litr(yylval.d); }
  | STRINGV   { $$ = new Litr(new string(yylval.s)); }
;
x:
    ID        { $$ = new Id(yylval.s); }
;

%%
