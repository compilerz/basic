%{
#include "mast.hpp"
#include "main.tab.hpp"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *lower(char*);
char *pop(char*, int);
char *unesc(char*);
char unescCode(char);
int line = 0;
%}

%option nounput
D   [0-9]
A   [A-Za-z_]
TYP [!@#$%&?]
INT {D}+
FLT ({D}*\.)?{D}+([eE]{INT})?
STR \"(\\.|[^"\\])*\"

%%
[ \t]                   ;
\'[^\n]*                ;
\r?\n                   { line++; return BR; }
(?i:rem)[\ \t]+[^\n]*   { yylval.s = yytext+4; return REM; }

(?i:let)        return LET;
(?i:print)      return PRINT;

(?i:and)  return AND;
(?i:or)   return OR;
(?i:xor)  return XOR;
(?i:imp)  return IMP;
(?i:eqv)  return EQV;
(?i:not)  return NOT;
(?i:mod)  return MOD;
\=      return EQ;
\<      return LT;
\>      return GT;
\<\=    return LE;
\>\=    return GE;
\<\>    return NE;
\+      return ADD;
\-      return SUB;
\*      return MUL;
\/      return DIV;
\\      return IDIV;
\^      return POW;
[,;:]   return yytext[0];
[\(\)]  return yytext[0];
(?i:true|false)   { yylval.b = strcmp(lower(yytext), "true") == 0; return BOOLEANV; }
{INT}             { yylval.i = atoi(yytext); return INTEGERV; }
{FLT}&            { yylval.l = atol(yytext); return LONGV; }
{FLT}             { yylval.f = (float) atof(yytext); return SINGLEV; }
{FLT}#            { yylval.d = atof(yytext); return DOUBLEV; }
{STR}             { yylval.s = unesc(pop(yytext+1, 1)); return STRINGV; }
{A}({A}|{D})*{TYP}?   { yylval.s = yytext; return ID; }
.

%%
// lower: lowercase characters in place
char *lower(char *s) {
  for(char *d=s; *d; d++)
    *d = tolower(*d);
  return s;
}

// pop: remove n chars from end of string
char *pop(char *s, int n) {
  s[strlen(s) - n] = '\0';
  return s;
}

// unesc: unescape string in place
char *unesc(char *s) {
  char buff[4];
  char *_s = s, *d = s;
  for(; *s; s++, d++) {
    if (*s != '\\') { *d = *s; continue; }
    if (s[1]>='0' && s[1]<='7') {
      strncpy(buff, s+1, 3);
      *d = (char) strtol(buff, NULL, 8);
      s += *d > 64? 3 : (*d > 8? 2 : 1);
    } else if (s[1] == 'x') {
      strncpy(buff, s+2, 2);
      *d = (char) strtol(buff, NULL, 16);
      s += strlen(buff)+1;
    } else {
      *d = unescCode(s[1]);
      s++;
    }
  }
  *d = '\0';
  return _s;
}

// unescCode: unescape character code
char unescCode(char c) { switch(c) {
  case 'a': return '\a';
  case 'b': return '\b';
  case 'e': return '\x1B';
  case 'f': return '\f';
  case 'n': return '\n';
  case 'r': return '\r';
  case 't': return '\t';
  case 'v': return '\v';
  default: return c;
}}
