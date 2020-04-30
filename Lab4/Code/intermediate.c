#include "intermediate.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

InterCodes connectInterCodes(int CodeNum, ...) {
    va_list arg_ptr;
    InterCodes former, latter;
    va_start(arg_ptr, CodeNum);
    former = va_arg(arg_ptr, InterCodes);
    int count = 1;
    while(count < CodeNum) {
        latter = va_arg(arg_ptr, InterCodes);
        InterCodes current = former;
        if(current == NULL) {
            former = latter;
            count ++;
            continue;
        }
        for(; current->next != NULL; current = current->next) ;
            current->next = latter;
        if(latter != NULL)
            latter->prev = current;
        count ++;
    }
    va_end(arg_ptr);
    return former;
}


void printTestCodes(InterCodes codes) {
    //printf("into\n");
    InterCodes current = codes;
    for(; current != NULL; current = current->next) {
        //printf("%d :", current->code.kind);
        if(current->code.kind == ASSIGN) {
            printOperand(current->code.u.assign.left);
            printf(" := ");
            printOperand(current->code.u.assign.right);
            printf("\n");
        }
        else if(current->code.kind == ADD) {
            printOperand(current->code.u.binop.result);
            printf(" := ");
            printOperand(current->code.u.binop.op1);
            printf(" + ");
            printOperand(current->code.u.binop.op2);
            printf("\n");
        }
        else if(current->code.kind == SUB) {
            printOperand(current->code.u.binop.result);
            printf(" := ");
            printOperand(current->code.u.binop.op1);
            printf(" - ");
            printOperand(current->code.u.binop.op2);
            printf("\n");
        }
        else if(current->code.kind == MUL) {
            printOperand(current->code.u.binop.result);
            printf(" := ");
            printOperand(current->code.u.binop.op1);
            printf(" * ");
            printOperand(current->code.u.binop.op2);
            printf("\n");
        }
        else if(current->code.kind == DIV) {
            printOperand(current->code.u.binop.result);
            printf(" := ");
            printOperand(current->code.u.binop.op1);
            printf(" / ");
            printOperand(current->code.u.binop.op2);
            printf("\n");
        }
        else if(current->code.kind == DEREF_L) {
            printf("*");
            printOperand(current->code.u.deref.left);
            printf(" := ");
            printOperand(current->code.u.deref.right);
            printf("\n");
        }
        else if(current->code.kind == DEREF_R) {
            printOperand(current->code.u.deref.left);
            printf(" := ");
            printf("*");
            printOperand(current->code.u.deref.right);
            printf("\n");
        }
        else if(current->code.kind == LABEL_C) {
            printf("LABEL ");
            printOperand(current->code.u.label.label);
            printf(" :\n");
        }
        else if(current->code.kind == GOTO_U) {
            printf("GOTO ");
            printOperand(current->code.u.label.label);
            printf("\n");
            //printf("and next is %d \n", current->next->code.kind);
        }
        else if(current->code.kind == GOTO_C) {
            printf("IF ");
            printOperand(current->code.u.goto_c.left);
            printRelop(current->code.u.goto_c.relopsym);
            printOperand(current->code.u.goto_c.right);
            printf(" GOTO ");
            printOperand(current->code.u.goto_c.label);
            printf("\n");
        }
        else if(current->code.kind == RELOP) {
            printOperand(current->code.u.relop.result);
            printf(" := ");
            printOperand(current->code.u.relop.op1);
            printRelop(current->code.u.relop.relopsym);
            printOperand(current->code.u.relop.op2);
            printf("\n");
        }
        else if(current->code.kind == RETURN) {
            printf("RETURN ");
            printOperand(current->code.u.ret.ret);
            printf("\n");
        }
        else if(current->code.kind == FUNC) {
            printf("FUNCTION %s :", current->code.u.func.name);
            printf("\n");
        }
        else if(current->code.kind == PARAM) {
            printf("PARAM v%d", current->code.u.param.no);
            printf("\n");
        }
        else if(current->code.kind == CALL) {
            printOperand(current->code.u.call.ret);
            printf(" := CALL %s", current->code.u.call.name);
            printf("\n");       
        }
        else if(current->code.kind == ARG) {
            printf("ARG ");
            printOperand(current->code.u.arg.arg);
            printf("\n");
        }
        else if(current->code.kind == READ) {
            printf("READ ");
            printOperand(current->code.u.rw.obj);
            printf("\n");
        }
        else if(current->code.kind == WRITE) {
            printf("WRITE ");
            printOperand(current->code.u.rw.obj);
            printf("\n");
        }
        else if(current->code.kind == DEC) {
            printf("DEC ");
            printOperand(current->code.u.dec.base); 
            printf(" %d", current->code.u.dec.length);
            printf("\n");
        }
    } 
}

void printRelop(Relop relopsym) {
    switch (relopsym) {
        case GT: printf(" > "); break;
        case GE: printf(" >= "); break;
        case LT: printf(" < "); break;
        case LE: printf(" <= "); break;
        case EQ: printf(" == "); break;
        case NE: printf(" != "); break;
    }



}

void printOperand(Operand op) {
    if(op->kind == CONSTANT) {
        printf("#%d", op->u.value);
    }
    else if(op->kind == VARIABLE) {
        printf("v%d", op->u.var_no);
    }
    else if(op->kind == TEMP) {
        printf("t%d", op->u.tmp_no);
    }
    else if(op->kind == ADDRESS) {
        printf("&v%d", op->u.var_no);
    }
    else if(op->kind == LABEL) {
        printf("label%d", op->u.label_no);
    }
}

int new_variable() {
    static int var_no = 0;
    var_no ++;
    return var_no;
}

Operand new_label() {
    static int label_no = 0;
    label_no ++;
    Operand l = (Operand)malloc(sizeof(Operand_));
    l->kind = LABEL;
    l->u.label_no = label_no;
    return l;
}

Operand new_temp() {
    static int temp_no = 0;
    temp_no ++;
    Operand tmp = (Operand)malloc(sizeof(Operand_));
    tmp->kind = TEMP;
    tmp->u.tmp_no = temp_no;
    return tmp;
}

InterCodes translateProgram(GTN* treeNode) {
    //printf("translate Program\n");
    //InterCodes o = translateExtDefList(treeNode->childrenList[0]);
    //printf("hello\n");
    //printTestCodes(o);
    return translateExtDefList(treeNode->childrenList[0]);
    //return o;
}


InterCodes translateExtDefList(GTN* treeNode) {
    //printf("translate ExtDefList\n");
    if(treeNode->childrenNum == 0) 
		return NULL;	
	else {
		return connectInterCodes(2, translateExtDef(treeNode->childrenList[0]), translateExtDefList(treeNode->childrenList[1]) );
	}
}

InterCodes translateExtDef(GTN* treeNode) {
    //printf("translate ExtDef\n");
	if(!strcmp(treeNode->childrenList[1]->unitName, "SEMI")) {
		return NULL;
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "ExtDecList")) {
		return NULL;
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "FunDec")) {
		if(!strcmp(treeNode->childrenList[2]->unitName, "CompSt")) {  // function definition
			return connectInterCodes(2, translateFunDec(treeNode->childrenList[1]), translateCompSt(treeNode->childrenList[2]) );
		}
		else if(!strcmp(treeNode->childrenList[2]->unitName, "SEMI")) {  //function declaration
			return NULL;
		}
			
	}
}

InterCodes translateFunDec(GTN* treeNode) {
    InterCodes func = (InterCodes)malloc(sizeof(InterCodes_));
    func->code.kind = FUNC;
    func->code.u.func.name = treeNode->childrenList[0]->lexValue;
    func->prev = NULL;
    func->next = NULL;

    if(treeNode->childrenNum == 3) {
        return func;
    }
    else if(treeNode->childrenNum == 4) {
        return connectInterCodes(2, func, translateVarList(treeNode->childrenList[2]));
    }

}

InterCodes translateVarList(GTN* treeNode) {
    if(treeNode->childrenNum == 1) {
        return translateParamDec(treeNode->childrenList[0]);
    }
    else if(treeNode->childrenNum == 3) {
        return connectInterCodes(2, translateParamDec(treeNode->childrenList[0]), translateVarList(treeNode->childrenList[2]) );
    }
}

InterCodes translateParamDec(GTN* treeNode) {
    Sym variable = searchSymbol(translateVarDec(treeNode->childrenList[1]));
    if(variable->var_Id == 0)
        variable->var_Id = new_variable();

    InterCodes param = (InterCodes)malloc(sizeof(InterCodes_));
    param->code.kind = PARAM;
    param->code.u.param.no = variable->var_Id; 
    param->prev = NULL;
    param->next = NULL;
    return param;
}

InterCodes translateCompSt(GTN* treeNode) {
    //printf("translate CompSt\n");
    return connectInterCodes(2, translateDefList(treeNode->childrenList[1]) ,translateStmtList(treeNode->childrenList[2]) );
}


InterCodes translateDefList(GTN* treeNode) {
    //printf("translate DefList\n");
    if(treeNode->childrenNum == 0) {
        return NULL;
    }
    else if(treeNode->childrenNum == 2) {
        return connectInterCodes(2, translateDef(treeNode->childrenList[0]), translateDefList(treeNode->childrenList[1]) );
    }
}

InterCodes translateDef(GTN* treeNode) {
    //printf("translate Def\n");
    return translateDecList(treeNode->childrenList[1]);
}

InterCodes translateDecList(GTN* treeNode) {
    //printf("translate DecList\n");
    if(treeNode->childrenNum == 1) {
        return translateDec(treeNode->childrenList[0]);
    }
        
    else if(treeNode->childrenNum == 3) {
        return connectInterCodes(2, translateDec(treeNode->childrenList[0]), translateDecList(treeNode->childrenList[2]) );
    }
}

InterCodes translateDec(GTN* treeNode) {
    //printf("translate Dec\n");
    if(treeNode->childrenNum == 1) {
        //printf("return NUll\n");
        GTN *varNode = treeNode->childrenList[0];
        if(varNode->childrenNum == 1) {
            Sym variable = searchSymbol(translateVarDec(varNode));
            if(variable->var_Id == 0)
                variable->var_Id = new_variable();
            Operand v = (Operand)malloc(sizeof(Operand_));
            v->kind = VARIABLE;
            v->u.var_no = variable->var_Id;
            
            if(variable->type->kind == BASIC)
                return NULL;
            // from now on translate structure 
            InterCodes deccode = (InterCodes)malloc(sizeof(InterCodes_));
            deccode->code.kind = DEC;
            deccode->code.u.dec.base = v;
            deccode->code.u.dec.length = getfieldlength(variable->type->u.structure.fieldlist);
            deccode->prev = NULL;
            deccode->next = NULL;
            return deccode;
        }
        else if(varNode->childrenNum == 4 ){
            Sym variable = searchSymbol(translateVarDec(varNode));
            if(variable->var_Id == 0)
                variable->var_Id = new_variable();
            Operand v = (Operand)malloc(sizeof(Operand_));
            v->kind = VARIABLE;
            v->u.var_no = variable->var_Id;
            int len = 4 * (variable->type->u.array.size);

            InterCodes deccode = (InterCodes)malloc(sizeof(InterCodes_));
            deccode->code.kind = DEC;
            deccode->code.u.dec.base = v;
            deccode->code.u.dec.length = len;
            deccode->prev = NULL;
            deccode->next = NULL;

            return deccode;
        }

    }
        
    else if(treeNode->childrenNum == 3) {
        Sym variable = searchSymbol(translateVarDec(treeNode->childrenList[0]));
        if(variable->var_Id == 0)
            variable->var_Id = new_variable();
        Operand v = (Operand)malloc(sizeof(Operand_));
        v->kind = VARIABLE;
        v->u.var_no = variable->var_Id;

        Operand t = new_temp();

        InterCodes code1 = translateExp(treeNode->childrenList[2], t);
        InterCodes code2 = (InterCodes)malloc(sizeof(InterCodes_));
        code2->code.kind = ASSIGN;
        code2->code.u.assign.left = v;
        code2->code.u.assign.right = t;
        code2->prev = NULL;
        code2->next = NULL;
        return connectInterCodes(2, code1, code2);
    }
}

int getfieldlength(FieldList target) {
    //printf("cal length\n");
    int length = 0;
    for(FieldList current = target; current != NULL; current= current->tail) {
        switch(current->type->kind) {
            case BASIC: length += 4; break;
            case ARRAY: length +=  current->type->u.array.size * 4; break;
            case STRUCTURE: length += getfieldlength(current->type->u.structure.fieldlist); break;
        }
    }
    return length;

}


char* translateVarDec(GTN* treeNode) {
    //printf("translate VarDec\n");
    if(treeNode->childrenNum == 1) {
        return treeNode->childrenList[0]->lexValue;
    }
        
    else if(treeNode->childrenNum == 4) {
        return translateVarDec(treeNode->childrenList[0]);
    }
}

InterCodes translateStmtList(GTN* treeNode) {
    //printf("translate StmtList\n");
    if(treeNode->childrenNum == 0)
        return NULL;
    else if(treeNode->childrenNum == 2) {
        return connectInterCodes(2, translateStmt(treeNode->childrenList[0]), translateStmtList(treeNode->childrenList[1]) );
    }
}

InterCodes translateStmt(GTN* treeNode) {
    if(treeNode->childrenNum == 2) {
		return translateExp(treeNode->childrenList[0], NULL);
	}
	else if(treeNode->childrenNum == 1) {
        return translateCompSt(treeNode->childrenList[0]);
	}
	else if(treeNode->childrenNum == 3) {
        Operand tmp = new_temp();
        InterCodes code1 = translateExp(treeNode->childrenList[1], tmp);
        InterCodes code2 = (InterCodes)malloc(sizeof(InterCodes_));
        code2->code.kind = RETURN;
        code2->code.u.ret.ret = tmp;
        code2->prev = NULL;
        code2->next = NULL;

        return connectInterCodes(2, code1, code2);
	} 
	else if(treeNode->childrenNum == 5 && !strcmp(treeNode->childrenList[0]->unitName, "IF")) {
	    Operand label1 = new_label();
        Operand label2 = new_label();

        InterCodes code1 = translateCond(treeNode->childrenList[2], label1, label2);
        InterCodes code2 = translateStmt(treeNode->childrenList[4]);

        InterCodes code3 = (InterCodes)malloc(sizeof(InterCodes_));
        code3->code.kind = LABEL_C;
        code3->code.u.label.label = label1;
        code3->prev = NULL;
        code3->next = NULL;

        InterCodes code4 = (InterCodes)malloc(sizeof(InterCodes_));
        code4->code.kind = LABEL_C;
        code4->code.u.label.label = label2;
        code4->prev = NULL;
        code4->next = NULL;

        return connectInterCodes(4, code1, code3, code2, code4);
		
	} 
	else if(treeNode->childrenNum == 5 && !strcmp(treeNode->childrenList[0]->unitName, "WHILE")) {
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();

        InterCodes code1 = translateCond(treeNode->childrenList[2], label2, label3);
        InterCodes code2 = translateStmt(treeNode->childrenList[4]);

        InterCodes code3= (InterCodes)malloc(sizeof(InterCodes_));
        code3->code.kind = LABEL_C;
        code3->code.u.label.label = label1;
        code3->prev = NULL;
        code3->next = NULL;

        InterCodes code4 = (InterCodes)malloc(sizeof(InterCodes_));
        code4->code.kind = LABEL_C;
        code4->code.u.label.label = label2;
        code4->prev = NULL;
        code4->next = NULL;

        InterCodes code5 = (InterCodes)malloc(sizeof(InterCodes_));
        code5->code.kind = GOTO_U;
        code5->code.u.label.label = label1;
        code5->prev = NULL;
        code5->next = NULL;

        InterCodes code6 = (InterCodes)malloc(sizeof(InterCodes_));
        code6->code.kind = LABEL_C;
        code6->code.u.label.label = label3;
        code6->prev = NULL;
        code6->next = NULL;

        return connectInterCodes(6, code3, code1, code4, code2, code5, code6);
	} 
	else if(treeNode->childrenNum == 7) {
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();

        InterCodes code1 = translateCond(treeNode->childrenList[2], label1, label2);
        InterCodes code2 = translateStmt(treeNode->childrenList[4]);
        InterCodes code3 = translateStmt(treeNode->childrenList[6]);
        InterCodes code4 = (InterCodes)malloc(sizeof(InterCodes_));
        code4->code.kind = LABEL_C;
        code4->code.u.label.label = label1;
        code4->prev = NULL;
        code4->next = NULL;

        InterCodes code5 = (InterCodes)malloc(sizeof(InterCodes_));
        code5->code.kind = GOTO_U;
        code5->code.u.label.label = label3;
        code5->prev = NULL;
        code5->next = NULL;

        InterCodes code6 = (InterCodes)malloc(sizeof(InterCodes_));
        code6->code.kind = LABEL_C;
        code6->code.u.label.label = label2;
        code6->prev = NULL;
        code6->next = NULL;

        InterCodes code7 = (InterCodes)malloc(sizeof(InterCodes_));
        code7->code.kind = LABEL_C;
        code7->code.u.label.label = label3;
        code7->prev = NULL;
        code7->next = NULL;

        return connectInterCodes(7, code1, code4, code2, code5, code6, code3, code7);

        
	}

}

int calstructOffset(GTN* treeNode, FieldList* ptrf) {
    int offset = 0;
    if(treeNode->childrenNum == 3) {
        offset += calstructOffset(treeNode->childrenList[0], ptrf);
        FieldList current = *ptrf;
        for(; current != NULL; current = current->tail) {
            if(!strcmp(current->name, treeNode->childrenList[2]->lexValue)) {
                break;
            }
        }
        offset += getfieldlength(current->tail);
        *ptrf = current->type->u.structure.fieldlist;
    }
    else if(treeNode->childrenNum == 1) {
        Sym s = searchSymbol(treeNode->childrenList[0]->lexValue);
        *ptrf = s->type->u.structure.fieldlist;
    }
    return offset;
}

InterCodes translateExp(GTN* treeNode, Operand place) {
    //printf("translate Exp\n");
    if(treeNode->childrenNum == 3) {
        if(!strcmp(treeNode->childrenList[0]->unitName, "Exp")) {
            if(!strcmp(treeNode->childrenList[1]->unitName, "ASSIGNOP")) { //assign expression
                //printf("assign expression\n");
                GTN* leftExpNode = treeNode->childrenList[0];
                if(leftExpNode->childrenNum == 1) {    // assign to a int variable
                    Sym variable = searchSymbol(leftExpNode->childrenList[0]->lexValue);
                    if(variable->var_Id == 0)
                        variable->var_Id = new_variable();
                    Operand v = (Operand)malloc(sizeof(Operand_));
                    v->kind = VARIABLE;
                    v->u.var_no = variable->var_Id;

                    Operand t = new_temp();

                    InterCodes code1 = translateExp(treeNode->childrenList[2], t);
                    InterCodes code2 = (InterCodes)malloc(sizeof(InterCodes_));
                    code2->code.kind = ASSIGN;
                    code2->code.u.assign.left = v;
                    code2->code.u.assign.right = t;
                    code2->prev = NULL;
                    code2->next = NULL;
                    return connectInterCodes(2, code1, code2);
                }   
                else if(leftExpNode->childrenNum == 4) {  //assign to a one-dimension array variable
                    //printf("start array val\n");
                    //printf("name %s\n", leftExpNode->childrenList[0]->lexValue);
                    if(leftExpNode->childrenList[0]->childrenNum != 1)
                    {
                        GTN * rootstruct = leftExpNode->childrenList[0];
                        while(rootstruct->childrenNum != 1)
                            rootstruct = rootstruct->childrenList[0];
                        
                        Sym variable = searchSymbol(rootstruct->childrenList[0]->lexValue);

                        if(variable->var_Id == 0)
                            variable->var_Id = new_variable();
                        Operand v = (Operand)malloc(sizeof(Operand_));
                        v->kind = VARIABLE;
                        v->u.var_no = variable->var_Id;

                        FieldList* ptrf = (FieldList*)malloc(sizeof(FieldList));
                        int off = calstructOffset(leftExpNode->childrenList[0], ptrf);

                        Operand offset = (Operand)malloc(sizeof(Operand_));
                        offset->kind = CONSTANT;
                        offset->u.value = off;
                        //printf(" %d \n", offset->u.value);

                        Operand baseaddr = (Operand)malloc(sizeof(Operand_));
                        baseaddr->kind = ADDRESS;
                        baseaddr->u.var_no = variable->var_Id;
                        
                        Operand aimaddr = new_temp();

                        InterCodes calAddr = (InterCodes)malloc(sizeof(InterCodes_));
                        calAddr->code.kind = ADD;
                        calAddr->code.u.binop.op1 = baseaddr;
                        calAddr->code.u.binop.op2 = offset;
                        calAddr->code.u.binop.result = aimaddr;
                        calAddr->prev = NULL;
                        calAddr->next = NULL;

                        Operand index = new_temp();
                        InterCodes indexCode = translateExp(leftExpNode->childrenList[2], index);

                        Operand length = (Operand)malloc(sizeof(Operand_));
                        length->kind = CONSTANT;
                        length->u.value = 4;
                        Operand newoffset = new_temp();

                        InterCodes calOffset = (InterCodes)malloc(sizeof(InterCodes_));
                        calOffset->code.kind = MUL;
                        calOffset->code.u.binop.op1 = index;
                        calOffset->code.u.binop.op2 = length;
                        calOffset->code.u.binop.result = newoffset;
                        calOffset->prev = NULL;
                        calOffset->next = NULL;

                        Operand finaladdr = new_temp();

                        InterCodes calfinalAddr = (InterCodes)malloc(sizeof(InterCodes_));
                        calfinalAddr->code.kind = ADD;
                        calfinalAddr->code.u.binop.op1 = aimaddr;
                        calfinalAddr->code.u.binop.op2 = newoffset;
                        calfinalAddr->code.u.binop.result = finaladdr;
                        calfinalAddr->prev = NULL;
                        calfinalAddr->next = NULL;
                            
                        Operand givenvalue = new_temp();
                        InterCodes valueCode = translateExp(treeNode->childrenList[2], givenvalue);
                        
                        InterCodes fillvalueCode = (InterCodes)malloc(sizeof(InterCodes_));
                        fillvalueCode->code.kind = DEREF_L;
                        fillvalueCode->code.u.deref.left = finaladdr;
                        fillvalueCode->code.u.deref.right = givenvalue;
                        fillvalueCode->prev = NULL;
                        fillvalueCode->next = NULL;

                        return connectInterCodes(6, calAddr, indexCode, calOffset, calfinalAddr, valueCode, fillvalueCode);
                    }
                    else
                    {
                        Sym variable = searchSymbol(leftExpNode->childrenList[0]->childrenList[0]->lexValue);
                        if(variable->var_Id == 0)
                            variable->var_Id = new_variable();
                        Operand v = (Operand)malloc(sizeof(Operand_));
                        v->kind = VARIABLE;
                        v->u.var_no = variable->var_Id;
                        //printf("break poitn\n");
                        Operand index = new_temp();
                        InterCodes indexCode = translateExp(leftExpNode->childrenList[2], index);

                        Operand length = (Operand)malloc(sizeof(Operand_));
                        length->kind = CONSTANT;
                        length->u.value = 4;
                        Operand offset = new_temp();

                        InterCodes calOffset = (InterCodes)malloc(sizeof(InterCodes_));
                        calOffset->code.kind = MUL;
                        calOffset->code.u.binop.op1 = index;
                        calOffset->code.u.binop.op2 = length;
                        calOffset->code.u.binop.result = offset;
                        calOffset->prev = NULL;
                        calOffset->next = NULL;


                        Operand baseaddr = (Operand)malloc(sizeof(Operand_));
                        baseaddr->kind = ADDRESS;
                        baseaddr->u.var_no = variable->var_Id;
                        Operand aimaddr = new_temp();

                        InterCodes calAddr = (InterCodes)malloc(sizeof(InterCodes_));
                        calAddr->code.kind = ADD;
                        calAddr->code.u.binop.op1 = baseaddr;
                        calAddr->code.u.binop.op2 = offset;
                        calAddr->code.u.binop.result = aimaddr;
                        calAddr->prev = NULL;
                        calAddr->next = NULL;

                        Operand givenvalue = new_temp();
                        InterCodes valueCode = translateExp(treeNode->childrenList[2], givenvalue);
                        
                        InterCodes fillvalueCode = (InterCodes)malloc(sizeof(InterCodes_));
                        fillvalueCode->code.kind = DEREF_L;
                        fillvalueCode->code.u.deref.left = aimaddr;
                        fillvalueCode->code.u.deref.right = givenvalue;
                        fillvalueCode->prev = NULL;
                        fillvalueCode->next = NULL;

                        return connectInterCodes(5, indexCode, calOffset, calAddr, valueCode, fillvalueCode);
                    }
                }
                else if(leftExpNode->childrenNum == 3) { //assign to a struct variable
                    //printf("hereo\n");
                    GTN * rootstruct = leftExpNode->childrenList[0];
                    while(rootstruct->childrenNum != 1)
                        rootstruct = rootstruct->childrenList[0];
                
                    Sym variable = searchSymbol(rootstruct->childrenList[0]->lexValue);

                    if(variable->var_Id == 0)
                        variable->var_Id = new_variable();
                    Operand v = (Operand)malloc(sizeof(Operand_));
                    v->kind = VARIABLE;
                    v->u.var_no = variable->var_Id;
                    
                    FieldList* ptrf = (FieldList*)malloc(sizeof(FieldList));
                    int off = calstructOffset(leftExpNode, ptrf);

                    Operand offset = (Operand)malloc(sizeof(Operand_));
                    offset->kind = CONSTANT;
                    offset->u.value = off;

                    Operand baseaddr = (Operand)malloc(sizeof(Operand_));
                    baseaddr->kind = ADDRESS;
                    baseaddr->u.var_no = variable->var_Id;
                
                    Operand aimaddr = new_temp();

                    InterCodes calAddr = (InterCodes)malloc(sizeof(InterCodes_));
                    calAddr->code.kind = ADD;
                    calAddr->code.u.binop.op1 = baseaddr;
                    calAddr->code.u.binop.op2 = offset;
                    calAddr->code.u.binop.result = aimaddr;
                    calAddr->prev = NULL;
                    calAddr->next = NULL;
                    //printf("here1\n");
                    Operand givenvalue = new_temp();
                    InterCodes getvalueCode = translateExp(treeNode->childrenList[2], givenvalue);

                    InterCodes fillvalueCode = (InterCodes)malloc(sizeof(InterCodes_));
                    fillvalueCode->code.kind = DEREF_L;
                    fillvalueCode->code.u.deref.left = aimaddr;
                    fillvalueCode->code.u.deref.right = givenvalue;
                    fillvalueCode->prev = NULL;
                    fillvalueCode->next = NULL;

                    return connectInterCodes(3, calAddr, getvalueCode, fillvalueCode);
                    
                }

            }
            else if(!strcmp(treeNode->childrenList[1]->unitName, "PLUS")) {
                Operand tmp1 = new_temp();
                Operand tmp2 = new_temp();

                InterCodes addexp1 = translateExp(treeNode->childrenList[0], tmp1);
                InterCodes addexp2 = translateExp(treeNode->childrenList[2], tmp2);

                InterCodes add = (InterCodes)malloc(sizeof(InterCodes_));
                add->code.kind = ADD;
                add->code.u.binop.result = place;
                add->code.u.binop.op1 = tmp1;
                add->code.u.binop.op2 = tmp2;
                add->prev = NULL;
                add->next = NULL;

                return connectInterCodes(3, addexp1, addexp2, add);
            }
            else if(!strcmp(treeNode->childrenList[1]->unitName, "MINUS")) {
                Operand tmp1 = new_temp();
                Operand tmp2 = new_temp();

                InterCodes subexp1 = translateExp(treeNode->childrenList[0], tmp1);
                InterCodes subexp2 = translateExp(treeNode->childrenList[2], tmp2);

                InterCodes sub = (InterCodes)malloc(sizeof(InterCodes_));
                sub->code.kind = SUB;
                sub->code.u.binop.result = place;
                sub->code.u.binop.op1 = tmp1;
                sub->code.u.binop.op2 = tmp2;
                sub->prev = NULL;
                sub->next = NULL;

                return connectInterCodes(3, subexp1, subexp2, sub);
            }
            else if(!strcmp(treeNode->childrenList[1]->unitName, "STAR")) {
                Operand tmp1 = new_temp();
                Operand tmp2 = new_temp();

                InterCodes mulexp1 = translateExp(treeNode->childrenList[0], tmp1);
                InterCodes mulexp2 = translateExp(treeNode->childrenList[2], tmp2);

                InterCodes mul = (InterCodes)malloc(sizeof(InterCodes_));
                mul->code.kind = MUL;
                mul->code.u.binop.result = place;
                mul->code.u.binop.op1 = tmp1;
                mul->code.u.binop.op2 = tmp2;
                mul->prev = NULL;
                mul->next = NULL;

                return connectInterCodes(3, mulexp1, mulexp2, mul);
            }
            else if(!strcmp(treeNode->childrenList[1]->unitName, "DIV")) {
                Operand tmp1 = new_temp();
                Operand tmp2 = new_temp();

                InterCodes divexp1 = translateExp(treeNode->childrenList[0], tmp1);
                InterCodes divexp2 = translateExp(treeNode->childrenList[2], tmp2);

                InterCodes div = (InterCodes)malloc(sizeof(InterCodes_));
                div->code.kind = DIV;
                div->code.u.binop.result = place;
                div->code.u.binop.op1 = tmp1;
                div->code.u.binop.op2 = tmp2;
                div->prev = NULL;
                div->next = NULL;

                return connectInterCodes(3, divexp1, divexp2, div);
            }
            else if(!strcmp(treeNode->childrenList[1]->unitName, "AND") ||
                !strcmp(treeNode->childrenList[1]->unitName, "OR") ||
                !strcmp(treeNode->childrenList[1]->unitName, "RELOP")) {
                Operand label1 = new_label();
                Operand label2 = new_label();
                Operand zero = (Operand)malloc(sizeof(Operand_));
                zero->kind = CONSTANT;
                zero->u.value = 0;
                InterCodes code0 = (InterCodes)malloc(sizeof(InterCodes_));
                code0->code.kind = ASSIGN;
                code0->code.u.assign.left = place;
                code0->code.u.assign.right = zero;
                code0->prev = NULL;
                code0->next = NULL;

                InterCodes code1 = translateCond(treeNode, label1, label2);

                InterCodes code2 = (InterCodes)malloc(sizeof(InterCodes_));
                code2->code.kind = LABEL_C;
                code2->code.u.label.label = label1;
                code2->prev = NULL;
                code2->next = NULL;

                Operand one = (Operand)malloc(sizeof(Operand_));
                one->kind = CONSTANT;
                one->u.value = 1;
                InterCodes code3 = (InterCodes)malloc(sizeof(InterCodes_));
                code3->code.kind = ASSIGN;
                code3->code.u.assign.left = place;
                code3->code.u.assign.right = one;
                code3->prev = NULL;
                code3->next = NULL;

                InterCodes code4 = (InterCodes)malloc(sizeof(InterCodes_));
                code4->code.kind = LABEL_C;
                code4->code.u.label.label = label2;
                code4->prev = NULL;
                code4->next = NULL;

                return connectInterCodes(5, code0, code1, code2, code3, code4);
            }
            else if(!strcmp(treeNode->childrenList[1]->unitName, "DOT") ) {
                
                GTN * rootstruct = treeNode->childrenList[0];
                while(rootstruct->childrenNum != 1)
                    rootstruct = rootstruct->childrenList[0];
                
                Sym variable = searchSymbol(rootstruct->childrenList[0]->lexValue);

                if(variable->var_Id == 0)
                    variable->var_Id = new_variable();
                Operand v = (Operand)malloc(sizeof(Operand_));
                v->kind = VARIABLE;
                v->u.var_no = variable->var_Id;

                FieldList* ptrf = (FieldList*)malloc(sizeof(FieldList));
                int off = calstructOffset(treeNode, ptrf);

                Operand offset = (Operand)malloc(sizeof(Operand_));
                offset->kind = CONSTANT;
                offset->u.value = off;
                //printf(" %d \n", offset->u.value);

                Operand baseaddr = (Operand)malloc(sizeof(Operand_));
                baseaddr->kind = ADDRESS;
                baseaddr->u.var_no = variable->var_Id;
                
                Operand aimaddr = new_temp();

                InterCodes calAddr = (InterCodes)malloc(sizeof(InterCodes_));
                calAddr->code.kind = ADD;
                calAddr->code.u.binop.op1 = baseaddr;
                calAddr->code.u.binop.op2 = offset;
                calAddr->code.u.binop.result = aimaddr;
                calAddr->prev = NULL;
                calAddr->next = NULL;
                    
                InterCodes getvalueCode = (InterCodes)malloc(sizeof(InterCodes_));
                getvalueCode->code.kind = DEREF_R;
                getvalueCode->code.u.deref.left = place;
                getvalueCode->code.u.deref.right = aimaddr;
                getvalueCode->prev = NULL;
                getvalueCode->next = NULL;

                return connectInterCodes(2, calAddr, getvalueCode);

            }
        }
        else {
            if(!strcmp(treeNode->childrenList[0]->unitName, "LP")) {
                return translateExp(treeNode->childrenList[1], place);
            }
        	else if(!strcmp(treeNode->childrenList[0]->unitName, "ID"))	{ // function without args
                if(!strcmp(treeNode->childrenList[0]->lexValue, "read")) {
                    InterCodes read = (InterCodes)malloc(sizeof(InterCodes_));
                    read->code.kind = READ;
                    read->code.u.rw.obj = place;
                    read->prev = NULL;
                    read->next = NULL;
                    return read;
                }
                else {
                    InterCodes callfunc = (InterCodes)malloc(sizeof(InterCodes_));
                    callfunc->code.kind = CALL;
                    callfunc->code.u.call.name = treeNode->childrenList[0]->lexValue;
                    callfunc->code.u.call.ret = place;
                    callfunc->prev = NULL;
                    callfunc->next = NULL;
                    return callfunc;
                }
            }
        }
    }
    else if(treeNode->childrenNum == 2) {
        if(!strcmp(treeNode->childrenList[0]->unitName, "MINUS")) {
            Operand tmp = new_temp();
            Operand zero = (Operand)malloc(sizeof(Operand_));
            zero->kind = CONSTANT;
            zero->u.value = 0;
            InterCodes expression = translateExp(treeNode->childrenList[1], tmp);
            InterCodes minus = (InterCodes)malloc(sizeof(InterCodes_));
            minus->code.kind = SUB;
            minus->code.u.binop.result = place;
            minus->code.u.binop.op1 = zero;
            minus->code.u.binop.op2 = tmp;
            minus->prev = NULL;
            minus->next = NULL;

            return connectInterCodes(2, expression, minus);
        }
        else if(!strcmp(treeNode->childrenList[0]->unitName, "NOT")) {
            Operand label1 = new_label();
            Operand label2 = new_label();
            Operand zero = (Operand)malloc(sizeof(Operand_));
            zero->kind = CONSTANT;
            zero->u.value = 0;
            InterCodes code0 = (InterCodes)malloc(sizeof(InterCodes_));
            code0->code.kind = ASSIGN;
            code0->code.u.assign.left = place;
            code0->code.u.assign.right = zero;
            code0->prev = NULL;
            code0->next = NULL;

            InterCodes code1 = translateCond(treeNode, label1, label2);

            InterCodes code2 = (InterCodes)malloc(sizeof(InterCodes_));
            code2->code.kind = LABEL_C;
            code2->code.u.label.label = label1;
            code2->prev = NULL;
            code2->next = NULL;

            Operand one = (Operand)malloc(sizeof(Operand_));
            one->kind = CONSTANT;
            one->u.value = 1;
            InterCodes code3 = (InterCodes)malloc(sizeof(InterCodes_));
            code3->code.kind = ASSIGN;
            code3->code.u.assign.left = place;
            code3->code.u.assign.right = one;
            code3->prev = NULL;
            code3->next = NULL;

            InterCodes code4 = (InterCodes)malloc(sizeof(InterCodes_));
            code4->code.kind = LABEL_C;
            code4->code.u.label.label = label2;
            code4->prev = NULL;
            code4->next = NULL;

            return connectInterCodes(5, code0, code1, code2, code3, code4);
        }
    }
    else if(treeNode->childrenNum == 4) {
        if(!strcmp(treeNode->childrenList[0]->unitName, "Exp")) {
            if(treeNode->childrenList[0]->childrenNum != 1)
            {
                GTN * rootstruct = treeNode->childrenList[0];
                while(rootstruct->childrenNum != 1)
                    rootstruct = rootstruct->childrenList[0];
                
                Sym variable = searchSymbol(rootstruct->childrenList[0]->lexValue);

                if(variable->var_Id == 0)
                    variable->var_Id = new_variable();
                Operand v = (Operand)malloc(sizeof(Operand_));
                v->kind = VARIABLE;
                v->u.var_no = variable->var_Id;

                FieldList* ptrf = (FieldList*)malloc(sizeof(FieldList));
                int off = calstructOffset(treeNode->childrenList[0], ptrf);

                Operand offset = (Operand)malloc(sizeof(Operand_));
                offset->kind = CONSTANT;
                offset->u.value = off;
                //printf(" %d \n", offset->u.value);

                Operand baseaddr = (Operand)malloc(sizeof(Operand_));
                baseaddr->kind = ADDRESS;
                baseaddr->u.var_no = variable->var_Id;
                
                Operand aimaddr = new_temp();

                InterCodes calAddr = (InterCodes)malloc(sizeof(InterCodes_));
                calAddr->code.kind = ADD;
                calAddr->code.u.binop.op1 = baseaddr;
                calAddr->code.u.binop.op2 = offset;
                calAddr->code.u.binop.result = aimaddr;
                calAddr->prev = NULL;
                calAddr->next = NULL;

                Operand index = new_temp();
                InterCodes indexCode = translateExp(treeNode->childrenList[2], index);

                Operand length = (Operand)malloc(sizeof(Operand_));
                length->kind = CONSTANT;
                length->u.value = 4;
                Operand newoffset = new_temp();

                InterCodes calOffset = (InterCodes)malloc(sizeof(InterCodes_));
                calOffset->code.kind = MUL;
                calOffset->code.u.binop.op1 = index;
                calOffset->code.u.binop.op2 = length;
                calOffset->code.u.binop.result = newoffset;
                calOffset->prev = NULL;
                calOffset->next = NULL;

                Operand finaladdr = new_temp();

                InterCodes calfinalAddr = (InterCodes)malloc(sizeof(InterCodes_));
                calfinalAddr->code.kind = ADD;
                calfinalAddr->code.u.binop.op1 = aimaddr;
                calfinalAddr->code.u.binop.op2 = newoffset;
                calfinalAddr->code.u.binop.result = finaladdr;
                calfinalAddr->prev = NULL;
                calfinalAddr->next = NULL;
                    
                InterCodes fillvalueCode = (InterCodes)malloc(sizeof(InterCodes_));
                fillvalueCode->code.kind = DEREF_R;
                fillvalueCode->code.u.deref.left = place;
                fillvalueCode->code.u.deref.right = finaladdr;
                fillvalueCode->prev = NULL;
                fillvalueCode->next = NULL;

                return connectInterCodes(5, calAddr, indexCode, calOffset, calfinalAddr, fillvalueCode);
            }
            else
            {
            //printf("right array %s\n", treeNode->childrenList[0]->childrenList[0]->lexValue);
                Sym variable = searchSymbol(treeNode->childrenList[0]->childrenList[0]->lexValue);
                if(variable->var_Id == 0)
                    variable->var_Id = new_variable();
                Operand v = (Operand)malloc(sizeof(Operand_));
                v->kind = VARIABLE;
                v->u.var_no = variable->var_Id;
                
                Operand index = new_temp();
                InterCodes indexCode = translateExp(treeNode->childrenList[2], index);

                Operand length = (Operand)malloc(sizeof(Operand_));
                length->kind = CONSTANT;
                length->u.value = 4;
                Operand offset = new_temp();

                InterCodes calOffset = (InterCodes)malloc(sizeof(InterCodes_));
                calOffset->code.kind = MUL;
                calOffset->code.u.binop.op1 = index;
                calOffset->code.u.binop.op2 = length;
                calOffset->code.u.binop.result = offset;
                calOffset->prev = NULL;
                calOffset->next = NULL;


                Operand baseaddr = (Operand)malloc(sizeof(Operand_));
                baseaddr->kind = ADDRESS;
                baseaddr->u.var_no = variable->var_Id;
                Operand aimaddr = new_temp();

                InterCodes calAddr = (InterCodes)malloc(sizeof(InterCodes_));
                calAddr->code.kind = ADD;
                calAddr->code.u.binop.op1 = baseaddr;
                calAddr->code.u.binop.op2 = offset;
                calAddr->code.u.binop.result = aimaddr;
                calAddr->prev = NULL;
                calAddr->next = NULL;
                    
                InterCodes fillvalueCode = (InterCodes)malloc(sizeof(InterCodes_));
                fillvalueCode->code.kind = DEREF_R;
                fillvalueCode->code.u.deref.left = place;
                fillvalueCode->code.u.deref.right = aimaddr;
                fillvalueCode->prev = NULL;
                fillvalueCode->next = NULL;

                return connectInterCodes(4, indexCode, calOffset, calAddr, fillvalueCode);
            }
        }
        else if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {  //function with args
            ArgList arglist = (ArgList)malloc(sizeof(ArgList_));
            arglist->op = NULL;
            arglist->prev = NULL;
            arglist->next = NULL;

            InterCodes args = translateArgs(treeNode->childrenList[2], arglist);
            //printf("args over\n");
            ArgList cur = arglist;
            for(; cur->prev != NULL; cur = cur->prev) ;
            arglist = cur;

            if(!strcmp(treeNode->childrenList[0]->lexValue, "write")) {
                InterCodes write = (InterCodes)malloc(sizeof(InterCodes_));
                write->code.kind = WRITE;
                write->code.u.rw.obj = arglist->op;
                write->prev = NULL;
                write->next = NULL;
                return connectInterCodes(2, args, write);
            }
            else {
                //printf("here\n");
                InterCodes argcodes = NULL;
                //printOperand(arglist->prev->op);
                

                for(ArgList current = arglist; current->op != NULL; current = current->next ) {
                    //printf("in circur\n");
                    InterCodes argcode = (InterCodes)malloc(sizeof(InterCodes_));
                    argcode->code.kind = ARG;
                    argcode->code.u.arg.arg = current->op;
                    argcode->prev = NULL;
                    argcode->next = NULL;
                    argcodes = connectInterCodes(2, argcodes, argcode);
                }
                InterCodes callcode = (InterCodes)malloc(sizeof(InterCodes_));
                callcode->code.kind = CALL;
                callcode->code.u.call.name = treeNode->childrenList[0]->lexValue;
                //printf(" value %s, callname %s\n", treeNode->childrenList[0]->lexValue, callcode->code.u.call.name);
                if(place == NULL)
                {
                    Operand tmp = new_temp();
                    place = tmp;
                }
                callcode->code.u.call.ret = place;
                callcode->prev = NULL;
                callcode->next = NULL;
                //printTestCodes(callcode);
                //printTestCodes(connectInterCodes(3, args, argcodes, callcode));
                return connectInterCodes(3, args, argcodes, callcode);


            }
        }
    }
    else if(treeNode->childrenNum == 1) {
        if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {
            Sym variable = searchSymbol(treeNode->childrenList[0]->lexValue);
            if(variable->var_Id == 0)
                variable->var_Id = new_variable();
            Operand v = (Operand)malloc(sizeof(Operand_));
            v->kind = VARIABLE;
            v->u.var_no = variable->var_Id;

            InterCodes code0 = (InterCodes)malloc(sizeof(InterCodes_));
            code0->code.kind = ASSIGN;
            code0->code.u.assign.left = place;
            code0->code.u.assign.right = v;
            code0->prev = NULL;
            code0->next = NULL;

            return code0;
        }
        else if(!strcmp(treeNode->childrenList[0]->unitName, "INT")) {
            Operand integer = (Operand)malloc(sizeof(Operand_));
            integer->kind = CONSTANT;
            integer->u.value = atoi(treeNode->childrenList[0]->lexValue);

            InterCodes code0 = (InterCodes)malloc(sizeof(InterCodes_));
            code0->code.kind = ASSIGN;
            code0->code.u.assign.left = place;
            code0->code.u.assign.right = integer;
            code0->prev = NULL;
            code0->next = NULL;
            //printf("int exp\n");
            return code0;
        }
        else if(!strcmp(treeNode->childrenList[0]->unitName, "FLOAT")) {
            Operand floater = (Operand)malloc(sizeof(Operand_));
            floater->kind = CONSTANT;
            floater->u.value = atof(treeNode->childrenList[0]->lexValue);

            InterCodes code0 = (InterCodes)malloc(sizeof(InterCodes_));
            code0->code.kind = ASSIGN;
            code0->code.u.assign.left = place;
            code0->code.u.assign.right = floater;
            code0->prev = NULL;
            code0->next = NULL;

            return code0;
        }
    }
}

InterCodes translateArgs(GTN* treeNode, ArgList arglist) {
    //printf("trans args\n");
    Operand t1 = new_temp();
    InterCodes code1 = translateExp(treeNode->childrenList[0], t1);
    Operand v = (Operand)malloc(sizeof(Operand_));
    ArgList arg = (ArgList)malloc(sizeof(ArgList_));

    GTN* expNode = treeNode->childrenList[0];
    if(expNode->childrenNum == 1) {
        if(!strcmp(expNode->childrenList[0]->unitName, "ID")) {
            Sym variable = searchSymbol(treeNode->childrenList[0]->childrenList[0]->lexValue);
            if(variable->type->kind == STRUCTURE) {
                v->kind = ADDRESS;
                v->u.var_no = variable->var_Id;
                arg->op = v;
            }
            else {
                v->kind = VARIABLE;
                v->u.var_no = variable->var_Id;
                arg->op = v;
            }
        }
        else {
            arg->op = t1;
        }
    }
    else {
        arg->op = t1;
    }

    

    arg->prev = NULL;
    arg->next = arglist;
    arglist->prev = arg;
    arglist = arg;
    if(treeNode->childrenNum == 1) {
        //printf("try arg\n");
        //printOperand(arglist->op);
        return code1;
    }
    else if(treeNode->childrenNum == 3) {
        InterCodes code2 = translateArgs(treeNode->childrenList[2], arglist);
        return connectInterCodes(2, code1, code2);
    }
}

InterCodes translateCond(GTN* treeNode, Operand label_true, Operand label_false) {
    //printf("translate Cond");
    if(!strcmp(treeNode->childrenList[1]->unitName, "RELOP")) {
        Operand t1 = new_temp(), t2 = new_temp();
        InterCodes code1 = translateExp(treeNode->childrenList[0], t1);
        InterCodes code2 = translateExp(treeNode->childrenList[2], t2);

        Relop relop = get_relop(treeNode->childrenList[1]->lexValue);
        InterCodes code3 = (InterCodes)malloc(sizeof(InterCodes_));
        code3->code.kind = GOTO_C;
        code3->code.u.goto_c.left = t1;
        code3->code.u.goto_c.right = t2;
        code3->code.u.goto_c.relopsym = relop;
        code3->code.u.goto_c.label = label_true;
        code3->prev = NULL;
        code3->next = NULL;

        InterCodes code4 = (InterCodes)malloc(sizeof(InterCodes_));
        code4->code.kind = GOTO_U;
        code4->code.u.label.label = label_false;
        code4->prev = NULL;
        code4->next = NULL;

        return connectInterCodes(4, code1, code2, code3, code4);
    }
    else if(!strcmp(treeNode->childrenList[0]->unitName, "NOT")) {
        return translateCond(treeNode->childrenList[1], label_false, label_true);
    }
    else if(!strcmp(treeNode->childrenList[1]->unitName, "AND")) {
        Operand label1 = new_label();

        InterCodes code1 = translateCond(treeNode->childrenList[0], label1, label_false);
        InterCodes code2 = translateCond(treeNode->childrenList[2], label_true, label_false);

        InterCodes code3 = (InterCodes)malloc(sizeof(InterCodes_));
        code3->code.kind = LABEL_C;
        code3->code.u.label.label = label1;
        code3->prev = NULL;
        code3->next = NULL;

        return connectInterCodes(3, code1, code3, code2);
    }
    else if(!strcmp(treeNode->childrenList[1]->unitName, "OR")) {
        Operand label1 = new_label();

        InterCodes code1 = translateCond(treeNode->childrenList[0], label_true, label1);
        InterCodes code2 = translateCond(treeNode->childrenList[2], label_true, label_false);

        InterCodes code3 = (InterCodes)malloc(sizeof(InterCodes_));
        code3->code.kind = LABEL_C;
        code3->code.u.label.label = label1;
        code3->prev = NULL;
        code3->next = NULL;

        return connectInterCodes(3, code1, code3, code2);
    }
    else {
        Operand t1 = new_temp();
        Operand zero = (Operand)malloc(sizeof(Operand_));
        zero->kind = CONSTANT;
        zero->u.value = 0;

        Relop relop = NE;

        InterCodes code1 = translateExp(treeNode, t1);
        InterCodes code2 = (InterCodes)malloc(sizeof(InterCodes_));
        code2->code.kind = GOTO_C;
        code2->code.u.goto_c.left = t1;
        code2->code.u.goto_c.right = zero;
        code2->code.u.goto_c.relopsym = relop;
        code2->code.u.goto_c.label = label_true;
        code2->prev = NULL;
        code2->next = NULL;

        InterCodes code3 = (InterCodes)malloc(sizeof(InterCodes_));
        code3->code.kind = GOTO_U;
        code3->code.u.label.label = label_false;
        code3->prev = NULL;
        code3->next = NULL;

        return connectInterCodes(3, code1, code2, code3);

    }


}

Relop get_relop(char* rel) {
    Relop relsym;
    if(!strcmp(rel, ">") )
        relsym = GT;
    else if(!strcmp(rel, ">="))
        relsym = GE;
    else if(!strcmp(rel, "<"))
        relsym = LT;
    else if(!strcmp(rel, "<="))
        relsym = LE;
    else if(!strcmp(rel, "=="))
        relsym = EQ;
    else if(!strcmp(rel, "!="))
        relsym = NE;
    return relsym;
}