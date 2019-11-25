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
  vector<Ast*> *as;
}
%token BR REM END
%token BOOLEANV INTEGERV LONGV SINGLEV DOUBLEV STRINGV ID
%token IF THEN ELSEIF ELSE ENDIF
%token WHILE WEND
%token LET PRINT
%left AND OR XOR IMP EQV
%left NOT
%left EQ LT GT LE GE NE
%left MOD
%left ADD SUB
%left MUL DIV IDIV
%left POW
%type<as> lins
%type<a> lin
%type<a> b if while
%type<a> elseifs
%type<a> s set io scall e fcall n x
%type<as> pexps
%type<as> es
%start p

%%
p:
    lins BR   { yyroot = new Blk(*$1); }
  |           { yyroot = NULL; }
;
lins:
    lins BR lin   { $1->push_back($3); $$ = $1; }
  | lin           { $$ = new vector<Ast*>({$1}); }
;
lin:
    b
;



b:
    if
  | while
  | s
;
if:
    IF e THEN s                           { $$ = new If($2, $4, new Nop()); }
  | IF e THEN s ELSE s                    { $$ = new If($2, $4, $6); }
  | IF e THEN BR b BR endif               { $$ = new If($2, $5, new Nop()); }
  | IF e THEN BR b BR ELSE BR b BR endif  { $$ = new If($2, $5, $9); }
  | IF e THEN BR b BR elseifs             { $$ = new If($2, $5, $7); }
;
while:
    WHILE e BR lins BR WEND      { $$ = new While($2, new Blk(*$4)); }
;

elseifs:
    ELSEIF e THEN BR b BR endif             { $$ = new If($2, $5, {}); }
  | ELSEIF e THEN BR b BR ELSE BR b endif   { $$ = new If($2, $5, $9); }
  | ELSEIF e THEN BR b BR elseifs           { $$ = new If($2, $5, $7); }
;
endif:
    END IF
  | ENDIF
;



s: 
    set
  | io
  | scall
;
set:
    LET x EQ e  { $$ = new Let($2, $4); }
  | x EQ e      { $$ = new Let($1, $3); }
;
io:
    PRINT pexps         { $2->push_back(new Litr("\n")); $$ = new Call("print", *$2); }
  | PRINT pexps ','     { $2->push_back(new Litr("\t")); $$ = new Call("print", *$2); }
  | PRINT pexps ';'     { $$ = new Call("print", *$2); }
;
scall:
    x               { $$ = new Call($1, {}); }
  | x es            { $$ = new Call($1, *$2); }
;

pexps:
    pexps ',' e   { $1->push_back(new Litr("\t")); $1->push_back($3); $$ = $1; }
  | pexps ';' e   { $1->push_back($3); $$ = $1; }
  | e             { $$ = new vector<Ast*>({$1}); }
  |               { $$ = new vector<Ast*>(); }
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
  | fcall
  | n
  | x
;
n:
    BOOLEANV  { $$ = new Litr((double) yylval.b); }
  | INTEGERV  { $$ = new Litr((double) yylval.i); }
  | LONGV     { $$ = new Litr((double) yylval.l); }
  | SINGLEV   { $$ = new Litr((double) yylval.f); }
  | DOUBLEV   { $$ = new Litr(yylval.d); }
  | STRINGV   { $$ = new Litr(new string(yylval.s)); }
;

fcall:
    x '(' ')'       { $$ = new Call($1, {}); }
  | x '(' es  ')'   { $$ = new Call($1, *$3); }
;
es:
    es ',' e    { $1->push_back($3); $$ = $1; }
  | e           { $$ = new vector<Ast*>({$1}); }
;
x:
    ID        { $$ = new Id(yylval.s); }
;

%%
