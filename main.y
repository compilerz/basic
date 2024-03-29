%{
#include "mast.hpp"
#include <string.h>

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
%token DECLARE DEF SUB FUNCTION
%token DIM REDIM SHARED STATIC TYPE
%token AS BOOLEAN INTEGER LONG SINGLE DOUBLE STRING
%token IF THEN ELSEIF ELSE ENDIF SELECT CASE
%token FOR TO STEP NEXT WHILE WEND DO UNTIL LOOP
%token LET CONST INPUT LINE PRINT OPEN CLOSE
%token OUTPUT RANDOM BINARY APPEND ACCESS READ WRITE
%token GOTO GOSUB RETURN EXIT
%token BOOLEANV INTEGERV LONGV SINGLEV DOUBLEV STRINGV ID
%token BR REM END
%left AND OR XOR IMP EQV
%left NOT
%left EQ LT GT LE GE NE
%left MOD
%left ADD SUB
%left MUL DIV IDIV
%left POW
%type<a> b l
%type<a> c proc define if select for while do
%type<a> elseifs cases case case_else
%type<a> s set io file branch scall
%type<a> e fcall n x
%type<as> pexps
%type<as> es
%start p

%%
p:
    b     { yyroot = $1; }
  |       { yyroot = NULL; }
;
b:
    b l   { $$ = ((Blk*)$1)->add($2); }
  | l     { $$ = new Blk({$1}); }
;
l:
    x ':' c BR  { $$ = $3; }
  | x ':' BR    {}
  | n c BR      { $$ = $2; }
  | n BR        {}
  | c BR        { $$ = $1; }
  | BR          {}
;

c:
    proc
  | define
  | if
  | select
  | for
  | while
  | do
  | s
;
proc:
    DECLARE SUB x defc                 { /* $$ = new Declare($3, $4); */ }
  | DECLARE FUNCTION x defc            { /* $$ = new Declare($3, $4); */ }
  | SUB x defc BR b END SUB            { /* $$ = new Proc($2, $3, $5); */ }
  | FUNCTION x defc BR b END FUNCTION  { /* $$ = new Proc($2, $3, $5); */ }
;
define:
    DIM defs                    {}
  | DIM SHARED defs             {}
  | REDIM es                    {}
  | SHARED defs                 {}
  | STATIC defs                 {}
  | TYPE x BR defb BR END TYPE  {}
;
if:
    IF e THEN s                     { $$ = new If($2, $4, new Nop()); }
  | IF e THEN s ELSE s              { $$ = new If($2, $4, $6); }
  | IF e THEN BR b endif            { $$ = new If($2, $5, new Nop()); }
  | IF e THEN BR b ELSE BR b endif  { $$ = new If($2, $5, $8); }
  | IF e THEN BR b elseifs          { $$ = new If($2, $5, $6); }
;
select:
    SELECT CASE e BR cases END SELECT    { /* $$ = new Select($3, $5); */ }
;
for:
    FOR x EQ e TO e BR b NEXT            { /* $$ = new For($2, $4, $6, $2, 1, $8); */ }
  | FOR x EQ e TO e BR b NEXT x          { /* $$ = new For($2, $4, $6, $11, 1, $8); */ }
  | FOR x EQ e TO e STEP e BR b NEXT     { /* $$ = new For($2, $4, $6, $2, $8, $10); */ }
  | FOR x EQ e TO e STEP e BR b NEXT x   { /* $$ = new For($2, $4, $6, $13, $8, $10); */ }
;
while:
    WHILE e BR b WEND      { $$ = new While($2, $4); }
;
do:
    DO WHILE e BR b LOOP   { /* $$ = new Do($3, new Litr(true), $5); */ }
  | DO UNTIL e BR b LOOP   { /* $$ = new Do(new Call("not", {$3}), new Litr(true), $5); */ }
  | DO BR b LOOP WHILE e   { /* $$ = new Do(new Litr(true), $7, $3); */ }
  | DO BR b LOOP UNTIL e   { /* $$ = new Do(new Litr(true), new Call("not", {$7}), $3); */ }
;

elseifs:
    ELSEIF e THEN BR b endif             { $$ = new If($2, $5, {}); }
  | ELSEIF e THEN BR b ELSE BR b endif   { $$ = new If($2, $5, $8); }
  | ELSEIF e THEN BR b elseifs           { $$ = new If($2, $5, $6); }
;
endif:
    END IF
  | ENDIF
;
cases:
    case cases  { /* $$ = $1->add($3); */ }
  | case_else   { /* $$ = new Asts({$1}); */ }
  | case        { /* $$ = $1->add($3); */ }
;
case:
    CASE e BR b        { /* $$ = new Case($2, $4); */ }
  | CASE e TO e BR b   { /* $$ = new CaseTo($2, $4, $6); */ }
;
case_else:
    CASE ELSE BR b     { /* $$ = new CaseElse($4); */ }
;



s: 
    set
  | io
  | file
  | branch
  | scall
;
set:
    LET x EQ e    { $$ = new Let($2, $4); }
  | CONST x EQ e  { $$ = new Let($2, $4); }
  | x EQ e        { $$ = new Let($1, $3); }
;
io:
    INPUT es            { /* $$ = new Input("", "", $2); */ }
  | INPUT ';' es        { /* $$ = new Input("", "?", $3); */ }
  | INPUT e ';' es      { /* $$ = new Input($2, "?", $4); */ }
  | LINE INPUT x        { /* $$ = new LineInput("", "", $3); */ }
  | LINE INPUT ';' x    { /* $$ = mew LineInput("", "?", $4); */ }
  | LINE INPUT e ';' x  { /* $$ = new LineInput($3, "?", $5); */ }
  | LINE INPUT e ',' x  { /* $$ = new LineInput($3, "", $5); */ }
  | PRINT pexps         { $2->push_back(new Litr("\n")); $$ = new Call("print", *$2); }
  | PRINT pexps ','     { $2->push_back(new Litr("\t")); $$ = new Call("print", *$2); }
  | PRINT pexps ';'     { $$ = new Call("print", *$2); }
;
file:
    OPEN e fmode facc AS e  { /* $$ = new Call("open", $6, $2, new Litr($3), new Litr($4)); */ }
  | OPEN e ',' e ',' e      { /* $$ = new Call("open", $4, $6, $2, new Litr("RW")); */ }
  | CLOSE es                { /* $$ = new Call("close", es); /* split?? */ }
  | CLOSE                   { /* $$ = new Call("closeall", {}); */ }
;
branch:
    GOTO lbl    { /* $$ = new Goto($2); */ }
  | GOSUB lbl   { /* $$ = new Gosub($2); */ }
  | RETURN      { /* $$ = new Return(""); */ }
  | RETURN lbl  { /* $$ = new Return($2); */ }
  | EXIT efrom  { /* $$ = new Exit((long) $2); */ }
;
scall:
    x               { $$ = new Call($1, {}); }
  | x es            { $$ = new Call($1, *$2); }
;

fmode:
    FOR OUTPUT  { /* $$ = "O"; */ }
  | FOR INPUT   { /* $$ = "I"; */ }
  | FOR RANDOM  { /* $$ = "R"; */ }
  | FOR BINARY  { /* $$ = "B"; */ }
  | FOR APPEND  { /* $$ = "A"; */ }
  |             { /* $$ = "I"; */ }
;
facc:
    ACCESS READ         { /* $$ = "R"; */ }
  | ACCESS WRITE        { /* $$ = "W"; */ }
  | ACCESS READ WRITE   { /* $$ = "RW"; */ }
  |                     { /* $$ = "R"; */ }
;
lbl:
    n
  | x
;
efrom:
    DEF       { /* $$ = 'D'; */ }
  | DO        { /* $$ = 'd'; */ }
  | FOR       { /* $$ = 'f'; */ }
  | SUB       { /* $$ = 'S'; */ }
  | FUNCTION  { /* $$ = 'F'; */ }
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
fcall:
    x '(' ')'       { $$ = new Call($1, {}); }
  | x '(' es  ')'   { $$ = new Call($1, *$3); }
;
n:
    BOOLEANV  { $$ = new Litr((double) yylval.b); }
  | INTEGERV  { $$ = new Litr((double) yylval.i); }
  | LONGV     { $$ = new Litr((double) yylval.l); }
  | SINGLEV   { $$ = new Litr((double) yylval.f); }
  | DOUBLEV   { $$ = new Litr(yylval.d); }
  | STRINGV   { $$ = new Litr(strdup(yylval.s)); }
;
x:
    ID        { $$ = new Id(yylval.s); }
;

es:
    es ',' e    { $1->push_back($3); $$ = $1; }
  | e           { $$ = new vector<Ast*>({$1}); }
;
defb:
    defb BR def   {}
  | def           {}
;
defc:
    '(' defs ')'  {}
  | '(' ')'       {}
  |               {}
;
defs:
    defs ',' def  {}
  | def           {}
;
def:
    x AS typ      {}
  | x             {}
;
typ:
    BOOLEAN   { /* $$ = 'b'; */ }
  | INTEGER   { /* $$ = 'i'; */ }
  | LONG      { /* $$ = 'l'; */ }
  | SINGLE    { /* $$ = 's'; */ }
  | DOUBLE    { /* $$ = 'd'; */ }
  | STRING    { /* $$ = 's'; */ }
;

%%
