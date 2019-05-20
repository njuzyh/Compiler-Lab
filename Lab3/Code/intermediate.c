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
    InterCodes current = codes;
    for(; current != NULL; current = current->next) {
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
}

int new_variable() {
    static int var_no = 0;
    var_no ++;
    return var_no;
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
    printf("translate Program\n");
    //InterCodes o = translateExtDefList(treeNode->childrenList[0]);
    //printf("hello\n");
    //printTestCodes(o);
    return translateExtDefList(treeNode->childrenList[0]);
    //return o;
}


InterCodes translateExtDefList(GTN* treeNode) {
    printf("translate ExtDefList\n");
    if(treeNode->childrenNum == 0) 
		return NULL;	
	else {
		return connectInterCodes(2, translateExtDef(treeNode->childrenList[0]), translateExtDefList(treeNode->childrenList[1]) );
	}
}

InterCodes translateExtDef(GTN* treeNode) {
    printf("translate ExtDef\n");
	if(!strcmp(treeNode->childrenList[1]->unitName, "SEMI")) {
		return NULL;
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "ExtDecList")) {
		return NULL;
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "FunDec")) {
		if(!strcmp(treeNode->childrenList[2]->unitName, "CompSt")) {  // function definition
			return translateCompSt(treeNode->childrenList[2]);
		}
		else if(!strcmp(treeNode->childrenList[2]->unitName, "SEMI")) {  //function declaration
			return NULL;
		}
			
	}
}


InterCodes translateCompSt(GTN* treeNode) {
    printf("translate CompSt\n");
    return connectInterCodes(2, translateDefList(treeNode->childrenList[1]) ,translateStmtList(treeNode->childrenList[2]) );
}


InterCodes translateDefList(GTN* treeNode) {
    printf("translate DefList\n");
    if(treeNode->childrenNum == 0) {
        return NULL;
    }
    else if(treeNode->childrenNum == 2) {
        return connectInterCodes(2, translateDef(treeNode->childrenList[0]), translateDefList(treeNode->childrenList[1]) );
    }
}

InterCodes translateDef(GTN* treeNode) {
    printf("translate Def\n");
    return translateDecList(treeNode->childrenList[1]);

}

InterCodes translateDecList(GTN* treeNode) {
    printf("translate DecList\n");
    if(treeNode->childrenNum == 1) {
        return translateDec(treeNode->childrenList[0]);
    }
        
    else if(treeNode->childrenNum == 3) {
        return connectInterCodes(2, translateDec(treeNode->childrenList[0]), translateDecList(treeNode->childrenList[2]) );
    }
}

InterCodes translateDec(GTN* treeNode) {
    printf("translate Dec\n");
    if(treeNode->childrenNum == 1) {
        //printf("return NUll\n");
        return NULL;
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

char* translateVarDec(GTN* treeNode) {
    printf("translate VarDec\n");
    if(treeNode->childrenNum == 1) {
        return treeNode->childrenList[0]->lexValue;
    }
        
    else if(treeNode->childrenNum == 4) {
        return translateVarDec(treeNode->childrenList[0]);
    }
}

InterCodes translateStmtList(GTN* treeNode) {
    printf("translate StmtList\n");
    if(treeNode->childrenNum == 0)
        return NULL;
    else if(treeNode->childrenNum == 2) {
        return connectInterCodes(2, translateStmt(treeNode->childrenList[0]), translateStmtList(treeNode->childrenList[1]) );
    }
}

InterCodes translateStmt(GTN* treeNode) {
    printf("translate Stmt\n");
    if(treeNode->childrenNum == 2) {
		return translateExp(treeNode->childrenList[0], NULL);
	}
	else if(treeNode->childrenNum == 1) {
	}
	else if(treeNode->childrenNum == 3) {
	
	} 
	else if(treeNode->childrenNum == 5 && !strcmp(treeNode->childrenList[0]->unitName, "IF")) {
	
		
	} 
	else if(treeNode->childrenNum == 5 && !strcmp(treeNode->childrenList[0]->unitName, "WHILE")) {

	} 
	else if(treeNode->childrenNum == 7) {

	}

}

InterCodes translateExp(GTN* treeNode, Operand place) {
    printf("translate Exp\n");
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
        }
        else {
            if(!strcmp(treeNode->childrenList[0]->unitName, "LP")) {
                return translateExp(treeNode->childrenList[1], place);
            }
        	else if(!strcmp(treeNode->childrenList[0]->unitName, "ID"))	{ // function without args
            
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
        }
    }
    else if(treeNode->childrenNum == 4) {
        if(!strcmp(treeNode->childrenList[0]->unitName, "Exp")) {
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
        else if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {  //function with args
        
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