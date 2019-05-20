#ifndef _INTERMEDIATE_H_
#define _INTERMEDIATE_H_
#include "grammartree.h"
#include "symbol.h"

typedef struct Operand_* Operand;
typedef struct Operand_ {
	enum {VARIABLE, CONSTANT, ADDRESS, TEMP} kind;
	union {
		int var_no;
		int tmp_no;
		int value;
	} u;

} Operand_;

typedef struct IntermediateCode {
	enum {ASSIGN, ADD, SUB, MUL, DIV, DEREF_L, DEREF_R } kind; 
	union {
		struct { Operand right, left; } assign;
		struct { Operand result, op1, op2; } binop;
		struct { Operand right, left; } deref;
	} u;

} InterCode;

typedef struct IntermediateCodes* InterCodes;
typedef struct IntermediateCodes {
	InterCode code;
	struct IntermediateCodes *prev, *next;
} InterCodes_;


InterCodes connectInterCodes(int CodeNum, ...);
void printTestCodes(InterCodes codes);
void printOperand(Operand op);

InterCodes translateProgram(GTN* treeNode);
InterCodes translateExtDefList(GTN* treeNode);
InterCodes translateExtDef(GTN* treeNode);
InterCodes translateCompSt(GTN* treeNode);
InterCodes translateDefList(GTN* treeNode);
InterCodes translateStmtList(GTN* treeNode);
InterCodes translateDef(GTN* treeNode);
InterCodes translateDecList(GTN* treeNode);
InterCodes translateDec(GTN* treeNode);
InterCodes translateStmt(GTN* treeNode);
InterCodes translateExp(GTN* treeNode, Operand place);
char* translateVarDec(GTN* treeNode);
int new_variable();
Operand new_temp();

#endif
