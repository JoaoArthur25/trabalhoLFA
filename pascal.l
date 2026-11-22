%{
#include "tokens.h"
%}

%option noyywrap
%option caseless
%option yylineno

DIGITO   [0-9]
LETRA    [a-zA-Z_]
ID       {LETRA}({LETRA}|{DIGITO})*

%%

"{"[^}]*"}"     {  }
"//".*          {  }
[ \t\r\n]+      {  }

"program"       { return T_PROGRAM; }
"var"           { return T_VAR; }
"procedure"     { return T_PROCEDURE; }
"function"      { return T_FUNCTION; }
"begin"         { return T_BEGIN; }
"end"           { return T_END; }
"if"            { return T_IF; }
"then"          { return T_THEN; }
"else"          { return T_ELSE; }
"while"         { return T_WHILE; }
"do"            { return T_DO; }
"integer"       { return T_INTEGER; }
"real"          { return T_REAL; }
"boolean"       { return T_BOOLEAN; }
"and"           { return T_AND; }
"or"            { return T_OR; }
"not"           { return T_NOT; }
"true"          { return T_TRUE; }
"false"         { return T_FALSE; }
"div"|"mod"     { return T_OPMULT; }

{DIGITO}+"."{DIGITO}+   { return T_NUM_REAL; }
{DIGITO}+               { return T_NUM_INT; }
{ID}                    { return T_ID; }

":="                    { return T_ATRIB; }
"<>"|"<="|">="|"="|"<"|">"   { return T_OPREL; }
"+"|"-"                 { return T_OPSOMA; }
"*"|"/"                 { return T_OPMULT; }
";"                     { return T_PONTO_VIRG; }
":"                     { return T_DOIS_PONTOS; }
","                     { return T_VIRGULA; }
"("                     { return T_ABRE_PAR; }
")"                     { return T_FECHA_PAR; }
"."                     { return T_PONTO; }

.                       { return T_ERRO; }

%%
