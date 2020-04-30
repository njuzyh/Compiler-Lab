%{
	#include "syntax.tab.h"
	#include <ctype.h>
%}
%option yylineno

DELIM [ \t\n]
WHITESPACE {DELIM}+
digit [0-9]
FLOAT ({digit}*\.?{digit}+|{digit}+\.)
INT {digit}+
ID [_a-zA-Z][_0-9a-zA-Z]*
Slash [/]
Star [*]
Annotation [^*/]

%%
"//".*"\n" { }
"/*"{Slash}*({Annotation}{Slash}*|{Star}|{Annotation})*"*/" {  }
{WHITESPACE} {  }
{INT} { yylval.gtn = newLexNode("INT", yylineno, yytext); return INT; }
{FLOAT} { yylval.gtn = newLexNode("FLOAT", yylineno, yytext); return FLOAT; }
";" { yylval.gtn = newLexNode("SEMI", yylineno, ""); return SEMI; }
"," { yylval.gtn = newLexNode("COMMA", yylineno, ""); return COMMA; }
"=" { yylval.gtn = newLexNode("ASSIGNOP", yylineno, ""); return ASSIGNOP; }
>|<|>=|<=|==|!= { yylval.gtn = newLexNode("RELOP", yylineno, yytext); return RELOP; }
int|float { yylval.gtn = newLexNode("TYPE", yylineno, yytext); return TYPE; }
"+" { yylval.gtn = newLexNode("PLUS", yylineno, ""); return PLUS; }
"-" { yylval.gtn = newLexNode("MINUS", yylineno, ""); return MINUS; }
"*" { yylval.gtn = newLexNode("STAR", yylineno, ""); return STAR; }
"/" { yylval.gtn = newLexNode("DIV", yylineno, ""); return DIV; }
"&&" { yylval.gtn = newLexNode("AND", yylineno, ""); return AND; }
"||" { yylval.gtn = newLexNode("OR", yylineno, ""); return OR; }
"." { yylval.gtn = newLexNode("DOT", yylineno, ""); return DOT; }
"!" { yylval.gtn = newLexNode("NOT", yylineno, ""); return NOT; }
"(" { yylval.gtn = newLexNode("LP", yylineno, ""); return LP; }
")" { yylval.gtn = newLexNode("RP", yylineno, ""); return RP; }
"[" { yylval.gtn = newLexNode("LB", yylineno, ""); return LB; }
"]" { yylval.gtn = newLexNode("RB", yylineno, ""); return RB; }
"{" { yylval.gtn = newLexNode("LC", yylineno, ""); return LC; }
"}" { yylval.gtn = newLexNode("RC", yylineno, ""); return RC; }
struct { yylval.gtn = newLexNode("STRUCT", yylineno, ""); return STRUCT;}
return { yylval.gtn = newLexNode("RETURN", yylineno, ""); return RETURN; }
if { yylval.gtn = newLexNode("IF", yylineno, ""); return IF; }
else { yylval.gtn = newLexNode("ELSE", yylineno, ""); return ELSE; }
while { yylval.gtn = newLexNode("WHILE", yylineno, ""); return WHILE; }
{ID} { yylval.gtn = newLexNode("ID", yylineno, yytext); return ID; }
. { wrong = 1; printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }


