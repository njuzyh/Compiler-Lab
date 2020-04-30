#ifndef _INTERMEDIATE_H_
#define _INTERMEDIATE_H_
#include "grammartree.h"
#include "symbol.h"

typedef enum Relop_ Relop;
enum Relop_ {GT, GE, LT, LE, EQ, NE} ;

typedef struct Operand_* Operand;
typedef struct Operand_ {
	enum {VARIABLE, CONSTANT, ADDRESS, TEMP, LABEL} kind;
	union {
		int var_no;
		int tmp_no;
		int value;
		int label_no;
	} u;

} Operand_;

typedef struct IntermediateCode {
	enum {ASSIGN, ADD, SUB, MUL, DIV, DEREF_L, DEREF_R, LABEL_C, GOTO_U, GOTO_C, RELOP, RETURN, 
		FUNC, PARAM, CALL, ARG, READ, WRITE, DEC } kind; 
	union {
		struct { Operand right, left; } assign;
		struct { Operand result, op1, op2; } binop;
		struct { Operand right, left; } deref;
		struct { Operand label; } label;
		struct { Operand left, right; Relop relopsym; Operand label; } goto_c;
		struct { Operand result, op1, op2; Relop relopsym; } relop;
		struct { Operand ret; } ret;
		struct { char* name; } func;
		struct { int no; } param;
		struct { char* name; Operand ret; } call;
		struct { Operand arg; } arg;
		struct { Operand obj; } rw;
		struct { Operand base; int length; } dec;
	} u; 

} InterCode;

typedef struct IntermediateCodes* InterCodes;
typedef struct IntermediateCodes {
	InterCode code;
	struct IntermediateCodes *prev, *next;
} InterCodes_;

typedef struct ArgList_* ArgList;
typedef struct ArgList_ {
	Operand op;
	struct ArgList_ *prev, *next;
} ArgList_;

InterCodes connectInterCodes(int CodeNum, ...);
void printTestCodes(InterCodes codes);
void printOperand(Operand op);
void printRelop(Relop relopsym);

InterCodes translateProgram(GTN* treeNode);
InterCodes translateExtDefList(GTN* treeNode);
InterCodes translateExtDef(GTN* treeNode);
InterCodes translateFunDec(GTN* treeNode);
InterCodes translateVarList(GTN* treeNode);
InterCodes translateParamDec(GTN* treeNode);
InterCodes translateCompSt(GTN* treeNode);
InterCodes translateDefList(GTN* treeNode);
InterCodes translateStmtList(GTN* treeNode);
InterCodes translateDef(GTN* treeNode);
InterCodes translateDecList(GTN* treeNode);
InterCodes translateDec(GTN* treeNode);
int getfieldlength(FieldList target);
InterCodes translateStmt(GTN* treeNode);
int calstructOffset(GTN* treeNode, FieldList* ptrf);
InterCodes translateExp(GTN* treeNode, Operand place);
InterCodes translateArgs(GTN* treeNode, ArgList arglist);
char* translateVarDec(GTN* treeNode);
int new_variable();
Operand new_temp();
Operand new_label();
InterCodes translateCond(GTN* treeNode, Operand label_true, Operand label_false);
Relop get_relop(char* rel);

#endif
