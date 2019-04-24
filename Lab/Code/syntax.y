%{
	#include <stdio.h>
	#include "grammartree.h"
	extern int yylex(void);
	extern int yyerror(char* msg);
	void printError(int lineNo, char* msg);
	extern int wrong;
%}

%union{
	GTN* gtn; 
	int integer;
	float floater;
}

%locations
/* declared tokens */

%token <gtn> SEMI COMMA
%token <gtn> LC RC
%right <gtn> ASSIGNOP
%left <gtn> OR
%left <gtn> AND
%left <gtn> RELOP
%left <gtn> PLUS MINUS
%left <gtn> STAR DIV
%right <gtn> UMINUS NOT
%left <gtn> LP RP LB RB DOT
%token <gtn> INT FLOAT
%token <gtn> TYPE
%token <gtn> STRUCT RETURN IF WHILE ID
%nonassoc <gtn> LOWER_THAN_ELSE
%nonassoc <gtn> ELSE


%type <gtn> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%%
/* High-level Definitions */
Program : ExtDefList { $$ = newGramNode("Program", 1, $1);  if(wrong == 0) outputNode($$, 0); }
	;
ExtDefList : ExtDef ExtDefList { $$ = newGramNode("ExtDefList", 2, $1, $2); }
	| { $$ = newGramNode("ExtDefList", 0); }
	;
ExtDef : Specifier ExtDecList SEMI { $$ = newGramNode("ExtDef", 3, $1, $2, $3); }
	| Specifier SEMI { $$ = newGramNode("ExtDef", 2, $1, $2); }
	| Specifier FunDec CompSt { $$ = newGramNode("ExtDef", 3, $1, $2, $3); }
	| Specifier error SEMI { wrong = 1; printError( $1->lineNo, "Wrong Extern Definiton\n"); }
	;
ExtDecList : VarDec { $$ = newGramNode("ExtDecList", 1, $1);}
	| VarDec COMMA ExtDecList { $$ = newGramNode("ExtDecList", 3, $1, $2, $3);}
	;

/* Specifiers */
Specifier : TYPE { $$ = newGramNode("Specifier", 1, $1);}
	| StructSpecifier { $$ = newGramNode("Specifier", 1, $1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC  { $$ = newGramNode("StructSpecifier", 5, $1, $2, $3, $4, $5); }
	| STRUCT Tag  { $$ = newGramNode("StructSpecifier", 2, $1, $2); }
	;
OptTag : ID  { $$ = newGramNode("OptTag", 1, $1); }
	| { $$ = newGramNode("OptTag", 0); }
	;
Tag : ID  { $$ = newGramNode("Tag", 1, $1); }
	;
/* Declarators */
VarDec : ID { $$ = newGramNode("VarDec", 1, $1); }
	| VarDec LB INT RB  { $$ = newGramNode("VarDec", 4, $1, $2, $3, $4); }
	;
FunDec : ID LP VarList RP  { $$ = newGramNode("FunDec", 4, $1, $2, $3, $4); }
	| ID LP RP { $$ = newGramNode("FunDec", 3, $1, $2, $3); }
	| ID error RP { wrong = 1; printError($1->lineNo, "Missing '('\n"); }
	;
VarList : ParamDec COMMA VarList  { $$ = newGramNode("VarList", 3, $1, $2, $3); }
	| ParamDec  { $$ = newGramNode("VarList", 1, $1); }
	;
ParamDec : Specifier VarDec  { $$ = newGramNode("ParamDec", 2, $1, $2); }
	;
/* Statements */
CompSt : LC DefList StmtList RC { $$ = newGramNode("CompSt", 4, $1, $2, $3, $4);}
	| error RC { wrong = 1; printf("Wrong CompSt\n"); }
	;
StmtList : Stmt StmtList { $$ = newGramNode("StmtList", 2, $1, $2); }
	| { $$ = newGramNode("StmtList", 0); }
	;
Stmt : Exp SEMI { $$ = newGramNode("Stmt", 2, $1, $2); }
	| CompSt { $$ = newGramNode("Stmt", 1, $1); }
	| RETURN Exp SEMI { $$ = newGramNode("Stmt", 3, $1, $2, $3); }
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = newGramNode("Stmt", 5, $1, $2, $3, $4, $5); }
	| IF LP Exp RP Stmt ELSE Stmt { $$ = newGramNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
	| WHILE LP Exp RP Stmt  { $$ = newGramNode("Stmt", 5, $1, $2, $3, $4, $5); }

	| error SEMI { wrong = 1; printError( $2->lineNo, "Wrong Statement\n"); }
	| RETURN Exp error SEMI { wrong = 1; printError( $2->lineNo, "Missing ';'\n"); }
	| IF LP error Stmt %prec LOWER_THAN_ELSE { wrong = 1; printError( $2->lineNo, "Missing ')'\n"); }
	| IF error RP Stmt %prec LOWER_THAN_ELSE { wrong = 1; printError( $3->lineNo, "Missing '('\n"); }
	| IF LP error Stmt ELSE Stmt { wrong = 1; printError( $2->lineNo, "Missing ')'\n"); }
	| IF error RP Stmt ELSE Stmt { wrong = 1; printError( $3->lineNo, "Missing '('\n"); }
	| WHILE error RP Stmt  { wrong = 1; printError( $3->lineNo, "Missing '('\n"); }
	| WHILE LP error Stmt  { wrong = 1; printError( $2->lineNo, "Missing ')'\n"); }
	| Exp error SEMI { wrong = 1; printError($1->lineNo, "Missing ';'\n");}
	;
/* Local Definitions */
DefList : Def DefList { $$ = newGramNode("DefList", 2, $1, $2); }
	| { $$ = newGramNode("DefList", 0);}
	;
Def : Specifier DecList SEMI { $$ = newGramNode("Def", 3, $1, $2, $3); }
	| Specifier error SEMI { wrong = 1; printError($1->lineNo, "Wrong Definition\n"); }
	;
DecList : Dec { $$ = newGramNode("DecList", 1, $1); }
	| Dec COMMA DecList { $$ = newGramNode("DecList", 3, $1, $2, $3); }
	;
Dec : VarDec { $$ = newGramNode("Dec", 1, $1); }
	| VarDec ASSIGNOP Exp { $$ = newGramNode("Dec", 3, $1, $2, $3); }
	;
/* Expressions */
Exp : Exp ASSIGNOP Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp AND Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp OR Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp RELOP Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp PLUS Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp MINUS Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp STAR Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp DIV Exp { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| LP Exp RP { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| MINUS Exp %prec UMINUS { $$ = newGramNode("Exp", 2, $1, $2); }
	| NOT Exp { $$ = newGramNode("Exp", 2, $1, $2); }
	| ID LP Args RP { $$ = newGramNode("Exp", 4, $1, $2, $3, $4); }
	| ID LP RP { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| Exp LB Exp RB { $$ = newGramNode("Exp", 4, $1, $2, $3, $4); }
	| Exp DOT ID { $$ = newGramNode("Exp", 3, $1, $2, $3); }
	| ID { $$ = newGramNode("Exp", 1, $1); }
	| INT { $$ = newGramNode("Exp", 1, $1); }
	| FLOAT { $$ = newGramNode("Exp", 1, $1); }
	;
Args : Exp COMMA Args { $$ = newGramNode("Args", 3, $1, $2, $3); }
	| Exp { $$ = newGramNode("Args", 1, $1); }
	;

%%
#include "lex.yy.c"
int yyerror(char* msg) {
	//fprintf(stderr, "Error type B at Line %d:", yylineno);
}

void printError(int lineNo, char* msg) {
	printf("Error type B at Line %d: %s", lineNo, msg);
}


