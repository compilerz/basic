%{
#include "mast.hpp"
#include "main.tab.hpp"
#include <stdlib.h>
%}
%option nounput

%%
[ \t]   {}
\r?\n   { return BR; }
[+-]?[0-9.]+  { yylval.f = (float) atof(yytext); return NUMBER; }
[+\-*/]       { yylval.c = yytext[0]; return OPERATOR; }
[()]          { return yytext[0]; }
.
%%
