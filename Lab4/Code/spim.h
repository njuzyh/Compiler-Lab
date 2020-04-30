#ifndef _SPIM_H_
#define _SPIM_H_
#include "intermediate.h"

typedef struct Variable_* Variable;
typedef struct Variable_ {
    char *varName;
    int offset;
    int regId;
    struct Variable_* next;
} Variable_;

typedef struct Register {
    char* regName;
    int used;
    Variable content;
} Register;

void printMIPS(InterCodes intercodes);
void initRegisters();
int get_reg();
void storeReg(Variable x);
int loadReg(Variable x);
void printAssignCode(InterCodes current);
void printAddCode(InterCodes current);
void printSubCode(InterCodes current);
void printMulCode(InterCodes current);
void printDivCode(InterCodes current);
void printDerefLCode(InterCodes current);
void printDerefRCode(InterCodes current);
void printLabelCode(InterCodes current);
void printGotoUCode(InterCodes current);
void printGotoCCode(InterCodes current);
void printReturnCode(InterCodes current);
void printFuncCode(InterCodes current);
void printReadCode(InterCodes current);
void printWriteCode(InterCodes current);
void printDecCode(InterCodes current);
void printCallCode(InterCodes current);
InterCodes printArgCode(InterCodes current);
void printParamCode(InterCodes current);

Variable handleOp(Operand current);
Variable searchVariable(char* name);


#endif