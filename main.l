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
(?i:end)                return END;

(?i:declare)    return DECLARE;
(?i:def)        return DEF;
(?i:sub)        return SUB;
(?i:function)   return FUNCTION;

(?i:dim)        return DIM;
(?i:redim)      return REDIM;
(?i:shared)     return SHARED;
(?i:static)     return STATIC;
(?i:type)       return TYPE;

(?i:as)         return AS;
(?i:boolean)    return BOOLEAN;
(?i:integer)    return INTEGER;
(?i:long)       return LONG;
(?i:single)     return SINGLE;
(?i:double)     return DOUBLE;
(?i:string)     return STRING;

(?i:if)         return IF;
(?i:then)       return THEN;
(?i:elseif)     return ELSEIF;
(?i:else)       return ELSE;
(?i:endif)      return ENDIF;
(?i:select)     return SELECT;
(?i:case)       return CASE;

(?i:for)        return FOR;
(?i:to)         return TO;
(?i:step)       return STEP;
(?i:next)       return NEXT;
(?i:while)      return WHILE;
(?i:wend)       return WEND;
(?i:do)         return DO;
(?i:until)      return UNTIL;
(?i:loop)       return LOOP;

(?i:let)        return LET;
(?i:const)      return CONST;
(?i:input)      return INPUT;
(?i:line)       return LINE;
(?i:print)      return PRINT;
(?i:open)       return OPEN;
(?i:close)      return CLOSE;

(?i:output)     return OUTPUT;
(?i:random)     return RANDOM;
(?i:binary)     return BINARY;
(?i:append)     return APPEND;
(?i:access)     return ACCESS;
(?i:read)       return READ;
(?i:write)      return WRITE;

(?i:goto)       return GOTO;
(?i:gosub)      return GOSUB;
(?i:return)     return RETURN;
(?i:exit)       return EXIT;

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
