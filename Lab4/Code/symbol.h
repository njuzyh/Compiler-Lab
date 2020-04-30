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
	int var_Id;
}Sym_;


typedef struct Type_ {
	enum {BASIC, ARRAY, STRUCTURE, FUNCTION} kind;
	union
	{
		int basic;
		struct { Type elem; int size; } array;
		struct { char* structname; FieldList fieldlist; } structure;
		struct { char* funcname; int paramnum; Type rettype; FieldList param; int isdefined; int decline; } function;
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

void handleProgram(GTN* program);
void handleExtDefList(GTN* treeNode);
void handleExtDef(GTN* treeNode);
Type handleSpecifier(GTN* treeNode);
void handleExtDecList(GTN* treeNode, Type spectype);
void handleFunDec(GTN* treeNode, Type spectype, int isdefinition);
void handleCompSt(GTN* treeNode, Type rettype);
void handleStructSpecifier(GTN* treeNode, Type buffer);
void handleTag(GTN* treeNode, Type buffer);
void handleOptTag(GTN* treeNode, Type buffer);
void handleDefList(GTN* treeNode, Type buffer);
void handleDef(GTN* treeNode, Type buffer);
void handleDecList(GTN* treeNode, Type spectype, Type buffer);
void handleDec(GTN* treeNode, Type spectype, Type buffer);
void handleVarDec(GTN* treeNode, Type spectype, Type buffer, int add);
FieldList handleVarList(GTN* treeNode, int add);
FieldList handleParamDec(GTN* treeNode, int add);
void handleStmtList(GTN* treeNode, Type rettype);
void handleStmt(GTN* treeNode, Type rettype);
Type handleExp(GTN* treeNode);
int checktype(Type type1, Type type2);
int isleftvalue(GTN* treeNode);
FieldList handleArgs(GTN* treeNode);
int checkargs(FieldList expected, FieldList get);
void checkUndefinedFunction();

void reportError(int errorType, int lineNo, char* msg);



void tranversal(GTN *treeNode);


#endif
