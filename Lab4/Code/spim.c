#include "spim.h"
#include "intermediate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int OFFSET = 0;
int locateReg = 0;
int argCount = 0;
int paramCount = 0;
Register registers[32];
Variable variableList = NULL;

void initRegisters() {
    for(int i = 0; i < 32; i++) {
        registers[i].regName = (char*)malloc(sizeof(char) * 4);
        registers[i].content = NULL;
        registers[i].used = 0;
        switch(i) {
            case 0: strcpy(registers[i].regName, "zero"); break;
            case 1: strcpy(registers[i].regName, "at"); break;
            case 2: strcpy(registers[i].regName, "v0"); break;
            case 3: strcpy(registers[i].regName, "v1"); break;
            case 4: strcpy(registers[i].regName, "a0"); break;
            case 5: strcpy(registers[i].regName, "a1"); break;
            case 6: strcpy(registers[i].regName, "a2"); break;
            case 7: strcpy(registers[i].regName, "a3"); break;
            case 8: strcpy(registers[i].regName, "t0"); break;
            case 9: strcpy(registers[i].regName, "t1"); break;
            case 10: strcpy(registers[i].regName, "t2"); break;
            case 11: strcpy(registers[i].regName, "t3"); break;
            case 12: strcpy(registers[i].regName, "t4"); break;
            case 13: strcpy(registers[i].regName, "t5"); break;
            case 14: strcpy(registers[i].regName, "t6"); break;
            case 15: strcpy(registers[i].regName, "t7"); break;
            case 16: strcpy(registers[i].regName, "s0"); break;
            case 17: strcpy(registers[i].regName, "s1"); break;
            case 18: strcpy(registers[i].regName, "s2"); break;
            case 19: strcpy(registers[i].regName, "s3"); break;
            case 20: strcpy(registers[i].regName, "s4"); break;
            case 21: strcpy(registers[i].regName, "s5"); break;
            case 22: strcpy(registers[i].regName, "s6"); break;
            case 23: strcpy(registers[i].regName, "s7"); break;
            case 24: strcpy(registers[i].regName, "t8"); break;
            case 25: strcpy(registers[i].regName, "t9"); break;
            case 26: strcpy(registers[i].regName, "k0"); break;
            case 27: strcpy(registers[i].regName, "k1"); break;
            case 28: strcpy(registers[i].regName, "gp"); break;
            case 29: strcpy(registers[i].regName, "sp"); break;
            case 30: strcpy(registers[i].regName, "s8"); break;
            case 31: strcpy(registers[i].regName, "ra"); break;
        }
    }
}

void printMIPS(InterCodes intercodes) {
    printf(".data\n");
    printf("_prompt: .asciiz \"Enter an integer:\"\n");
    printf("_ret: .asciiz \"\\n\"\n");
    printf(".globl main\n");
    printf(".text\n");
    printf("read:\n");
    printf("li $v0, 4\n");
    printf("la $a0, _prompt\n");
    printf("syscall\n");
    printf("li $v0, 5\n");
    printf("syscall\n");
    printf("jr $ra\n");
    printf("\nwrite:\n");
    printf("li $v0, 1\n");
    printf("syscall\n");
    printf("li $v0, 4\n");
    printf("la $a0, _ret\n");
    printf("syscall\n");
    printf("move $v0, $0\n");
    printf("jr $ra\n");

    InterCodes current = intercodes;
    for(; current != NULL; current = current->next) {
        switch(current->code.kind) {
            case ASSIGN:
                printAssignCode(current); break;
            case ADD:
                printAddCode(current); break;
            case SUB:
                printSubCode(current); break;
            case MUL:
                printMulCode(current); break;
            case DIV:
                printDivCode(current); break;
            case DEREF_L:
                printDerefLCode(current); break;
            case DEREF_R:
                printDerefRCode(current); break;
            case LABEL_C:
                printLabelCode(current); break;
            case GOTO_U:
                printGotoUCode(current); break;
            case GOTO_C:
                printGotoCCode(current); break;
            //case RELOP
            case RETURN:
                printReturnCode(current); break;
		    case FUNC:
                printFuncCode(current); break;
            case READ:
                printReadCode(current); break;
            case WRITE:
                printWriteCode(current); break;
            case CALL:
                printCallCode(current); break;
            case PARAM:
                printParamCode(current); break;
            case ARG:
                current = printArgCode(current); break;
            case DEC:
                printDecCode(current); break;
            default:
                printf("Illegal intercode\n");
                exit(-1);
        }

    }



}

void printAssignCode(InterCodes current)
{
    if(current->code.u.assign.right->kind == CONSTANT)
    {
        //printf("cons\n");
        Variable x = handleOp(current->code.u.assign.left);
        int index = x->regId;
        printf("li $%s, %d\n", registers[index].regName, current->code.u.assign.right->u.value);
        storeReg(x);
    }
    else
    {
        
        Variable x = handleOp(current->code.u.assign.left);
        Variable y = handleOp(current->code.u.assign.right);
        int index_x = x->regId;
        int index_y = y->regId;
        printf("move $%s, $%s\n", registers[index_x].regName, registers[index_y].regName);
        storeReg(x);
    }
}

void printAddCode(InterCodes current) {
    if(current->code.u.binop.op2->kind == CONSTANT)
    {
        Variable y = handleOp(current->code.u.binop.op1);
        Variable x = handleOp(current->code.u.binop.result);
        int index_x = x->regId;
        int index_y = y->regId;
        printf("addi $%s, $%s, %d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.binop.op2->u.value);
        storeReg(x);
    }
    else
    {
        Variable y = handleOp(current->code.u.binop.op1);
        Variable z = handleOp(current->code.u.binop.op2);
        Variable x = handleOp(current->code.u.binop.result);
        int index_x = x->regId, index_y = y->regId, index_z = z->regId;
        printf("add $%s, $%s, $%s\n", registers[index_x].regName, registers[index_y].regName, registers[index_z].regName);
        storeReg(x);
    }
}

void printSubCode(InterCodes current) {
    if(current->code.u.binop.op2->kind == CONSTANT)
    {
        Variable y = handleOp(current->code.u.binop.op1);
        Variable x = handleOp(current->code.u.binop.result);
        int index_x = x->regId;
        int index_y = y->regId;
        printf("addi $%s, $%s, -%d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.binop.op2->u.value);
        storeReg(x);
    }
    else
    {
        Variable y = handleOp(current->code.u.binop.op1);
        Variable z = handleOp(current->code.u.binop.op2);
        Variable x = handleOp(current->code.u.binop.result);
        int index_x = x->regId, index_y = y->regId, index_z = z->regId;
        printf("sub $%s, $%s, $%s\n", registers[index_x].regName, registers[index_y].regName, registers[index_z].regName);
        storeReg(x);
    }
}

void printMulCode(InterCodes current) {
    Variable y = handleOp(current->code.u.binop.op1);
    Variable z = handleOp(current->code.u.binop.op2);
    Variable x = handleOp(current->code.u.binop.result);
    int index_x = x->regId, index_y = y->regId, index_z = z->regId;
    printf("mul $%s, $%s, $%s\n", registers[index_x].regName, registers[index_y].regName, registers[index_z].regName);
    storeReg(x);
}

void printDivCode(InterCodes current) {
    Variable y = handleOp(current->code.u.binop.op1);
    Variable z = handleOp(current->code.u.binop.op2);
    Variable x = handleOp(current->code.u.binop.result);
    int index_x = x->regId, index_y = y->regId, index_z = z->regId;
    printf("div $%s, $%s\n", registers[index_y].regName, registers[index_z].regName);
    printf("mflo $%s\n", registers[index_x].regName);
    storeReg(x);
}

void printDerefLCode(InterCodes current) {
    Variable y = handleOp(current->code.u.deref.right);
    Variable x = handleOp(current->code.u.deref.left);
    int index_x = x->regId, index_y = y->regId;
    printf("sw $%s, 0($%s)\n", registers[index_y].regName, registers[index_x].regName);
}

void printDerefRCode(InterCodes current) {
    Variable y = handleOp(current->code.u.deref.right);
    Variable x = handleOp(current->code.u.deref.left);
    int index_x = x->regId, index_y = y->regId;
    printf("lw $%s, 0($%s)\n", registers[index_x].regName, registers[index_y].regName);
    storeReg(x);
}

void printLabelCode(InterCodes current) {
    printf("label%d:\n", current->code.u.label.label->u.label_no);
}

void printGotoUCode(InterCodes current) {
    printf("j label%d\n", current->code.u.label.label->u.label_no);
}

void printGotoCCode(InterCodes current) {
    Variable y = handleOp(current->code.u.goto_c.right);
    Variable x = handleOp(current->code.u.goto_c.left);
    int index_x = x->regId, index_y = y->regId;
    switch(current->code.u.goto_c.relopsym) {
        case GT: printf("bgt $%s, $%s, label%d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.goto_c.label->u.label_no); break;
        case GE: printf("bge $%s, $%s, label%d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.goto_c.label->u.label_no); break;
        case LT: printf("blt $%s, $%s, label%d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.goto_c.label->u.label_no); break;
        case LE: printf("ble $%s, $%s, label%d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.goto_c.label->u.label_no); break;
        case EQ: printf("beq $%s, $%s, label%d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.goto_c.label->u.label_no); break;
        case NE: printf("bne $%s, $%s, label%d\n", registers[index_x].regName, registers[index_y].regName, current->code.u.goto_c.label->u.label_no); break;
    }
}

void printReturnCode(InterCodes current) {
    Variable x = handleOp(current->code.u.ret.ret);
    int index_x = x->regId;
    printf("move $v0, $%s\n", registers[index_x].regName);
    printf("move $sp, $fp\n");
    printf("lw $ra, -4($sp)\n");
    printf("lw $fp, -8($sp)\n");
    printf("jr $ra\n");
    paramCount = 0;
}

void printFuncCode(InterCodes current) {
    OFFSET = -8;
    variableList = NULL;
    printf("\n%s:\n", current->code.u.func.name);
    printf("addi $sp, $sp, -8\n");
    printf("sw $ra, 4($sp)\n");
    printf("sw $fp, 0($sp)\n");
    printf("addi $fp, $sp, 8\n");
}

void printReadCode(InterCodes current)
{
    Variable x = handleOp(current->code.u.rw.obj);
    int index_x = x->regId;
    printf("addi $sp, $sp, -4\n");
    printf("sw $ra, 0($sp)\n");
    printf("jal read\n");
    printf("lw $ra, 0($sp)\n");
    printf("addi $sp, $sp, 4\n");
    printf("move $%s, $v0\n", registers[index_x].regName);
    storeReg(x);
}

void printWriteCode(InterCodes current)
{
    Variable x = handleOp(current->code.u.rw.obj);
    int index_x = x->regId;
    printf("move $a0, $%s\n", registers[index_x].regName);
    printf("addi $sp, $sp, -4\n");
    printf("sw $ra, 0($sp)\n");
    printf("jal write\n");
    printf("lw $ra, 0($sp)\n");
    printf("addi $sp, $sp, 4\n");
}

void printDecCode(InterCodes current) {
    char* name = malloc(20);
    if(current->code.u.dec.base->kind == VARIABLE)
    {
        sprintf(name, "v%d", current->code.u.dec.base->u.var_no); 
    }
    else if(current->code.u.dec.base->kind == TEMP)
    {
        sprintf(name, "t%d", current->code.u.dec.base->u.tmp_no); 
    }
    Variable var = (Variable)malloc(sizeof(Variable_));
    var->varName = name;
    var->next = variableList;
    var->regId = -1;
    OFFSET -= current->code.u.dec.length;
    printf("addi $sp, $sp, -%d\n", current->code.u.dec.length);
    var->offset = OFFSET;
    variableList = var;
    //return var;
}

void printCallCode(InterCodes current)
{
    printf("addi $sp, $sp, -4\n");
    printf("sw, $ra, 0($sp)\n");
    printf("jal %s\n", current->code.u.call.name);
    printf("lw $ra, 0($sp)\n");
    printf("addi $sp, $sp, 4\n");
    char* name = malloc(20);
    sprintf(name, "t%d", current->code.u.call.ret->u.var_no); 
    //printf("null %s\n", name);
    Variable aim = searchVariable(name);
    Variable var = (Variable)malloc(sizeof(Variable_));
    var->varName = name;
    var->next = variableList;
    var->regId = -1;
    OFFSET -= 4;
    printf("addi $sp, $sp, -4\n");
    var->offset = OFFSET;
    variableList = var;
    //printf("move $%s, $v0\n", registers[var->regId].regName);
    printf("sw $v0, %d($fp)\n", var->offset);
    argCount = 0;
}

InterCodes printArgCode(InterCodes current) {
    InterCodes c = current;
    for(; c->code.kind == ARG; c = c->next)
        argCount += 1;
    int minus = 0;
    if(argCount - 5 > 0)
    {
        minus = argCount - 5;
        printf("subu $sp, $sp, %d\n", minus*4);
    }
    int i = argCount;
    for(InterCodes c1 = c->prev; c1->code.kind == ARG; c1 = c1->prev)
    {
        char* name = malloc(20);
        if(c1->code.u.arg.arg->kind == VARIABLE) {
        sprintf(name, "v%d", c1->code.u.arg.arg->u.var_no); 
        }
        else if(c1->code.u.arg.arg->kind == TEMP) {
            sprintf(name, "t%d", c1->code.u.arg.arg->u.tmp_no); 
        }
        Variable aim = searchVariable(name);
        if(aim == NULL) {
            exit(-1);
        }
        else {
            if(i == argCount)
                printf("lw $a0, %d($fp)\n", aim->offset);
            else if(i == argCount - 1)
                printf("lw $a1, %d($fp)\n", aim->offset);
            else if(i == argCount - 2)
                printf("lw $a2, %d($fp)\n", aim->offset);
            else if(i == argCount - 3)
                printf("lw $a3, %d($fp)\n", aim->offset);
            else
            {
                printf("lw $t0, %d($fp)\n", aim->offset);
                printf("sw $t0, %d($sp)\n", 4*(argCount-4-i));
            }     
        }
        i = i - 1;
    }
    return c->prev;
}

void printParamCode(InterCodes current) {
    paramCount += 1;
    char* name = malloc(20);
    sprintf(name, "v%d", current->code.u.param.no); 
    Variable var = (Variable)malloc(sizeof(Variable_));
    var->varName = name;
    var->next = variableList;
    var->regId = -1;
    OFFSET -= 4;
    printf("addi $sp, $sp, -4\n");
    switch(paramCount) {
        case 1: printf("sw $a0, 0($sp)\n"); break;
        case 2: printf("sw $a1, 0($sp)\n"); break;
        case 3: printf("sw $a2, 0($sp)\n"); break;
        case 4: printf("sw $a3, 0($sp)\n"); break;
        default: 
            printf("lw $t0, %d($fp)\n", 4*(paramCount-5) + 4);
            printf("sw $t0, 0($sp)\n");     
            break;
    }
    var->offset = OFFSET;
    variableList = var;
}

Variable searchVariable(char* name)
{
    Variable temp = variableList;
	while(temp != NULL) {
 		if(!strcmp(name, temp->varName))
			return temp;
		temp = temp->next;
	}
	return temp;
}

Variable handleOp(Operand current)
{
    //printf("handle op %d\n", current->kind);
    char* name = malloc(20);
    if(current->kind == VARIABLE)
    {
        sprintf(name, "v%d", current->u.var_no); 
    }
    else if(current->kind == TEMP)
    {
        sprintf(name, "t%d", current->u.tmp_no); 
    }
    else if(current->kind == ADDRESS) {
        sprintf(name, "v%d", current->u.var_no); 
        Variable aim = searchVariable(name);
        int index = get_reg();
        printf("la $%s, %d($fp)\n", registers[index].regName, aim->offset);
        aim->regId = index;
        return aim;
    }
    else if(current->kind == CONSTANT) {
        int index = get_reg();
        printf("li $%s, %d\n", registers[index].regName, current->u.value);
        Variable aim = (Variable)malloc(sizeof(Variable_));
        aim->regId = index;
        return aim;
    }
    //printf("name %s\n", name);
    Variable aim = searchVariable(name);
    if(aim == NULL)
    {
        Variable var = (Variable)malloc(sizeof(Variable_));
        var->varName = name;
        var->next = variableList;
        var->regId = -1;
        OFFSET -= 4;
        printf("addi $sp, $sp, -4\n");
        var->offset = OFFSET;
        variableList = var;
        int index = get_reg();
        var->regId = index;
        registers[index].used = 1;
        registers[index].content = var;
        return var;
    }
    loadReg(aim);
    return aim;
}

int loadReg(Variable x) {
    int index = get_reg();
    x->regId = index;
    registers[index].used = 1;
    registers[index].content = x;
    printf("lw $%s, %d($fp)\n", registers[index].regName, x->offset);
    return index;
}

void storeReg(Variable x)
{
    int index = x->regId;
    printf("sw $%s, %d($fp)\n", registers[index].regName, x->offset);
    x->regId = -1;
    registers[index].content = NULL;
    registers[index].used = 0;
}

int get_reg()
{
    int index = (locateReg % 18) + 8;
    locateReg ++;
    return index;
}