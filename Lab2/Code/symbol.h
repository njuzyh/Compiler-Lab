#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#include "grammartree.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Sym_* Sym;

typedef struct Sym_ {
	char* name;
	Type type;
	Sym next;
}Sym_;


typedef struct Type_ {
	enum {BASIC, ARRAY, STRUCTURE, FUNCTION} kind;
	union
	{
		int basic;
		struct { Type elem; int size; } array;
		FieldList structure;
		struct { int paramnum; Type rettype; FieldList param; } function;
	}u;
}Type_;

typedef struct FieldList_ {
	char* name;
	Type type;
	FieldList tail;
}FieldList_;

unsigned int hash_pjw(char* name);
void init_symbolTable();
void insertSymbol(char *name, Type type);
Sym searchSymbol(char *name);
void printSymTab();

void startProgram(GTN* program);
void handleExtDefList(GTN* treeNode);
void handleExtDef(GTN* treeNode);
Type handleSpecifier(GTN* treeNode);
void handleExtDecList(GTN* treeNode, Type spectype);
void handleFunDec(GTN* treeNode, Type spectype);
void handleCompSt(GTN* treeNode);
void handleStructSpecifier(GTN* treeNode, Type buffer);
Type handleTag(GTN* treeNode);
Type handleOptTag(GTN* treeNode);
void handleDefList(GTN* treeNode, Type buffer);
void handleDef(GTN* treeNode, Type buffer);
void handleDecList(GTN* treeNode, Type spectype, Type buffer);
void handleDec(GTN* treeNode, Type spectype, Type buffer);
void handleVarDec(GTN* treeNode, Type spectype, Type buffer);
FieldList handleVarList(GTN* treeNode);
FieldList handleParamDec(GTN* treeNode);
void handleStmtList(GTN* treeNode);
void handleStmt(GTN* treeNode);
void handleExp(GTN* treeNode);

void reportError(int errorType, int lineNo, char* msg);



void tranversal(GTN *treeNode);


#endif
