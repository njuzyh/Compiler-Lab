#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#define SymTabSize 16384
extern Sym symbolTable[SymTabSize];
extern GTN* programRoot;

unsigned int hash_pjw(char* name) {
	unsigned int val = 0, i;
	for(; *name; ++name) {
		val = (val << 2) + *name;
		if(i = val & ~0x3fff)
			val = (val^(i >> 12)) & 0x3fff;
	}
	return val;
}

void init_symbolTable() {
	for(int i = 0; i < SymTabSize; i++)
		symbolTable[i] = NULL;
 
}

void insertSymbol(char *name, Type type) {
	Sym symbol = (Sym)malloc(sizeof(Sym_));
	symbol->name = name;
	symbol->type = type;
	symbol->next = NULL;
	
	unsigned int location = hash_pjw(name);
	Sym temp = symbolTable[location];
	if(temp == NULL) {
		symbolTable[location] = symbol;
		return;
	}
	symbol->next = symbolTable[location];
	symbolTable[location] = symbol;

}

Sym searchSymbol(char *name) {
	unsigned int location = hash_pjw(name);
	Sym temp = symbolTable[location];
	while(temp != NULL) {
 		if(!strcmp(name, temp->name))
			return temp;
		temp = temp->next;
	}
	return temp;
}

void printSymTab() {
	for(int i = 0; i < SymTabSize; i++) {
		if(symbolTable[i] != NULL) {
			//printf("get one\n");
			Sym current = symbolTable[i];
			while(current != NULL) {
				printf("%s\n",symbolTable[i]->name);
				current = current->next;
			}
		}
	}
}

void startProgram(GTN* program) {
	printf("startProgram\n");
	handleExtDefList(program->childrenList[0]);


}

void handleExtDefList(GTN* treeNode) {
	printf("start handle ExtDefList\n");
	if(treeNode->childrenNum == 0) 
		return;	
	else {
		handleExtDef(treeNode->childrenList[0]);
		handleExtDefList(treeNode->childrenList[1]);
	}
}	

void handleExtDef(GTN* treeNode) {
	printf("start handle ExtDef\n");
	if(!strcmp(treeNode->childrenList[1]->unitName, "SEMI")) {
		handleSpecifier(treeNode->childrenList[0]);
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "ExtDecList")) {
		Type spectype = handleSpecifier(treeNode->childrenList[0]);
		handleExtDecList(treeNode->childrenList[1], spectype);
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "FunDec")) {
		Type spectype = handleSpecifier(treeNode->childrenList[0]);
		handleFunDec(treeNode->childrenList[1], spectype);
		handleCompSt(treeNode->childrenList[2]);		
	}
}

Type handleSpecifier(GTN* treeNode) {
	printf("start handle Specifier\n");
	Type buffer = (Type) malloc(sizeof(Type_));
	if(!strcmp(treeNode->childrenList[0]->unitName, "TYPE")) {
		buffer->kind = BASIC;
		if(!strcmp(treeNode->childrenList[0]->lexValue, "int")) {
			buffer->u.basic = 0;    // 0 means int
		}
		else if(!strcmp(treeNode->childrenList[0]->lexValue, "float")) {
			buffer->u.basic = 1;    // 1 means float
		}
	}
	else if(!strcmp(treeNode->childrenList[0]->unitName, "StructSpecifier")) {
		buffer->kind = STRUCTURE;
		handleStructSpecifier(treeNode->childrenList[0], buffer);
	}

	return buffer;

}

void handleExtDecList(GTN* treeNode, Type spectype) {
	printf("start handle ExtDecList\n");
	if(treeNode->childrenNum == 1) {
		handleVarDec(treeNode->childrenList[0], spectype, NULL);
	}
	else if(treeNode->childrenNum == 3) {
		handleVarDec(treeNode->childrenList[0], spectype, NULL);
		handleExtDecList(treeNode->childrenList[2], spectype);
	}


}

void handleFunDec(GTN* treeNode, Type spectype) {
	printf("start handle FunDec\n");
	if(treeNode->childrenNum == 3) {
		Type func = (Type) malloc(sizeof(Type_));
		func->kind = FUNCTION;
		func->u.function.paramnum = 0;
		func->u.function.rettype = spectype;
		Sym funcSym = searchSymbol(treeNode->childrenList[0]->lexValue);
		if(funcSym == NULL)
			insertSymbol(treeNode->childrenList[0]->lexValue, func);
		else 
			reportError(4, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
	}
	else if(treeNode->childrenNum == 4) {
		Type func = (Type) malloc(sizeof(Type_));
		func->kind = FUNCTION;
		func->u.function.paramnum = 0;
		func->u.function.rettype = spectype;
		func->u.function.param = handleVarList(treeNode->childrenList[2]);
		for(FieldList current = func->u.function.param; current != NULL; current = current->tail)
			func->u.function.paramnum ++;
		Sym funcSym = searchSymbol(treeNode->childrenList[0]->lexValue);
		if(funcSym == NULL)
			insertSymbol(treeNode->childrenList[0]->lexValue, func);
		else 
			reportError(4, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
	}

}

FieldList handleVarList(GTN* treeNode)
{
	printf("start handle VarList\n");
	//FieldList paramList = (FieldList) malloc(sizeof(FieldList_));
	if(treeNode->childrenNum == 1) {
		FieldList param = handleParamDec(treeNode->childrenList[0]);
		return param;
	}
	else if(treeNode->childrenNum == 3){
		FieldList firstparam = handleParamDec(treeNode->childrenList[0]);
		FieldList restparam = handleVarList(treeNode->childrenList[2]);
		firstparam->tail = restparam;
		return firstparam;
	}
}

FieldList handleParamDec(GTN* treeNode) {
	printf("start handle ParamDec\n");
	FieldList param = (FieldList) malloc(sizeof(FieldList_));
	param->type = handleSpecifier(treeNode->childrenList[0]);
	//don't need a name here, param name is inserted into SymbolTable in 'handleVarDec'
	handleVarDec(treeNode->childrenList[1], param->type, NULL);
	param->tail = NULL;
	return param;
}

	
void handleCompSt(GTN* treeNode) {
	printf("start handle CompSt\n");
	handleDefList(treeNode->childrenList[1]);
	handleStmtList(treeNode->childrenList[2]);
}

void handleStmtList(GTN* treeNode) {
	printf("start handle StmtList\n");
	if(treeNode->childrenNum == 0)
		return;
	else if(treeNode->childrenNum == 2) {
		handleStmt(treeNode->childrenList[0]);
		handleStmtList(treeNode->childrenList[1]);
	}
}

void handleStmt(GTN* treeNode) {
	printf("start handle Stmt\n");
	if(treeNode->childrenNum == 2) {
		handleExp(treeNode->childrenList[0]);
	}
	else if(treeNode->childrenNum == 1) {
		handleCompSt(treeNode->childrenList[0]);
	}
	else if(treeNode->childrenNum == 3) {
		int i;
	} 

}

void handleExp(GTN* treeNode) {
	printf("start handle Exp\n");
	if(treeNode->childrenNum == 3) {
		if(!strcmp(treeNode->childrenList[0]->unitName, "Exp"))
		{
			if(!strcmp(treeNode->childrenList[1]->unitName, "AND"))
			{
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "OR")) {
			
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "RELOP")) {
			
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "PLUS")) {
			
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "MINUS")) {
			
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "STAR")) {
			
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "DIV")) {
			
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "DOT")) {
			
			}
		}
		else
		{
			if(!strcmp(treeNode->childrenList[0]->unitName, "LP"))
			{
				handleExp(treeNode->childrenList[1]);
			}
			else if(!strcmp(treeNode->childrenList[0]->unitName, "ID"))	
			{
			}
		}
		
		handleExp(treeNode->childrenList[0]);
	}
	else if(treeNode->childrenNum == 2) {
		if(!strcmp(treeNode->childrenList[0]->unitName, "MINUS")) {
			
		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "NOT")) {
			
		}
		handleExp(treeNode->childrenList[1]);
	}
	else if(treeNode->childrenNum == 4) {
		if(!strcmp(treeNode->childrenList[0]->unitName, "Exp")) {
			
		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {
			
		}
		handleExp(treeNode->childrenList[0]);
	}
	else if(treeNode->childrenNum == 1) {
		if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {
			
		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "INT")) {
			
		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "FLOAT")) {
			
		}
	}
}


void handleStructSpecifier(GTN* treeNode, Type buffer) {
	printf("start handle StructSpecifier\n");
	if(treeNode->childrenNum == 2) {
		Type tagType = handleTag(treeNode->childrenList[1]);
		if(tagType != NULL) 
			buffer = tagType;
		else
			buffer = NULL;
	}
	else if(treeNode->childrenNum == 5) {
		Type structType = handleOptTag(treeNode->childrenList[1]);
		handleDefList(treeNode->childrenList[3], structType);
	}
}

Type handleTag(GTN* treeNode) {
	printf("start handle Tag\n");
	Sym structSym = searchSymbol(treeNode->childrenList[0]->lexValue);
	if(structSym == NULL) {
		reportError(17, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
		return NULL;
	}
	else {
		return structSym->type;
	}
}

Type handleOptTag(GTN* treeNode) {
	printf("start handle OptTag\n");

	if(treeNode->childrenNum == 0) {
		Type newtype = (Type)malloc(sizeof(Type_));
		return newtype;
	}
	else if(treeNode->childrenNum == 1) {
		Sym structSym = searchSymbol(treeNode->childrenList[0]->lexValue);
		if(structSym == NULL) {
			Type newtype = (Type)malloc(sizeof(Type_));
			insertSymbol(treeNode->childrenList[0]->lexValue, newtype);
			return newtype;
		}
		else {
			reportError(16, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
			return NULL;
		}
	}
}



void handleDefList(GTN* treeNode, Type buffer) {
	printf("start handle DefList\n");
	if(treeNode->childrenNum == 0) 
		return;	
	else if(treeNode->childrenNum == 2){
		handleDef(treeNode->childrenList[0], buffer);
		handleDefList(treeNode->childrenList[1], buffer);
	}

}

void handleDef(GTN* treeNode, Type buffer) {
	printf("start handle Def\n");
	Type spectype = handleSpecifier(treeNode->childrenList[0]);
	handleDecList(treeNode->childrenList[1], spectype, buffer);
}


void handleDecList(GTN* treeNode, Type spectype, Type buffer) {
	printf("start handle DecList\n");
	if(treeNode->childrenNum == 1) 
		handleDec(treeNode->childrenList[0], spectype, buffer);
	else if(treeNode->childrenNum == 3){
		handleDec(treeNode->childrenList[0], spectype, buffer);
		handleDecList(treeNode->childrenList[2], spectype, buffer);
	}

}

void handleDec(GTN* treeNode, Type spectype, Type buffer) {
	printf("start handle Dec\n");
        handleVarDec(treeNode->childrenList[0], spectype, buffer);
}

void handleVarDec(GTN* treeNode, Type spectype, Type buffer) {
	printf("start handle VarDec\n");
	if(treeNode->childrenNum == 1) {
		FieldList field_var = (FieldList)malloc(sizeof(FieldList_));
		field_var->name = treeNode->childrenList[0]->lexValue;
		field_var->type = spectype;
		field_var->tail = NULL;

		if(buffer != NULL)
		{
			FieldList temp = buffer->u.structure;
			while(temp != NULL)
			{
				if(strcmp(temp->name, field_var->name) == 0)
				{
					reportError(15, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
					return ;
				}
				temp = temp->tail;
			}
			if(buffer->u.structure == NULL)
				buffer->u.structure = field_var;
			else
			{
				field_var->tail = buffer->u.structure;
				buffer->u.structure = field_var;
			}
		}
		else
		{
			Sym sym = searchSymbol(treeNode->childrenList[0]->lexValue);
			if(sym == NULL) 
			{
				//printf("ready insert %s %d\n", field_var->name, field_var->type->kind);
				insertSymbol(field_var->name, field_var->type);
			}
			else {
				reportError(3, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
				return ;
			}
		}		
		
	}
	else if(treeNode->childrenNum == 4){
		Type mytype = (Type)malloc(sizeof(Type_));
		mytype->kind = ARRAY;
		mytype->u.array.size = atoi(treeNode->childrenList[2]->lexValue);
		mytype->u.array.elem = spectype;
		handleVarDec(treeNode->childrenList[0], mytype, buffer);		
	}
}










void traversal(GTN* treeNode) {
	



	if(treeNode->lex == 1) {
		if(!strcmp(treeNode->unitName, "ID")) {
			int x;
		}
	}
		
	else {
		if(treeNode->childrenNum == 0)
			return;
		if(!strcmp(treeNode->unitName, "ExtDef")) {
			int a;


		}
		for(int i = 0; i < treeNode->childrenNum; i++) {
			//tranversal(gramNode->childrenList[i]);
			int p;
		}
	}
}	


void reportError(int errorType, int lineNo, char* msg) {
	printf("Error type %d at Line %d: ",  errorType, lineNo);
	switch(errorType) {
		case 1:
			printf("Undefined variable \"%s\".\n", msg);
			break;
		case 2:
			printf("Undefined function \"%s\".\n", msg);
			break;
		case 3:
			printf("Redefined variable \"%s\".\n", msg);
			break;
		case 4:
			printf("Redefined function \"%s\".\n", msg);
			break;
		case 5:
			printf("Type mismatched for assignment.\n");
			break;
		case 6:
			printf("The left-hand side of an assignment must be a varia-ble.\n");
			break;
		case 7:
			printf("Type mismatched for operands.\n");
			break;
		case 8:
			printf("Type mismatched for return.\n");
			break;
		case 9:
			printf("Function \"%s\" is not applicable for arguments.\n", msg);
			break;
		case 10:
			printf("\"%s\" is not an array.\n", msg);
			break;
		case 11:
			printf("\"%s\" is not a function.\n", msg);
			break;
		case 12:
			printf("\"%s\" is not an integer.\n", msg);
			break;
		case 13:
			printf("Illegal use of '.'.\n");
			break;
		case 14:
			printf("Non-existent field \"%s\".\n", msg);
			break;
		case 15:
			printf("Redefined field \"%s\".\n", msg);
			break;
		case 16:
			printf("Duplicated name \"%s\".\n", msg);
			break;
		case 17:
			printf("Undefined structure \"%s\".\n", msg);
			break;
	}
}



