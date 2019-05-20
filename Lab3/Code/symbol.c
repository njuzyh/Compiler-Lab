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
	symbol->var_Id = 0;
	
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

void handleProgram(GTN* program) {
	//printf("startProgram\n");
	handleExtDefList(program->childrenList[0]);
	checkUndefinedFunction();
}

void checkUndefinedFunction() {
	for(int i = 0; i < SymTabSize; i++) {
		if(symbolTable[i] != NULL) {
			Sym current = symbolTable[i];
			while(current != NULL) {
				if(current->type->kind == FUNCTION && current->type->u.function.isdefined == 0)
					reportError(18, current->type->u.function.decline, current->type->u.function.funcname);
				current = current->next;
			}
		}
	}
}

void handleExtDefList(GTN* treeNode) {
	//printf("start handle ExtDefList\n");
	if(treeNode->childrenNum == 0) 
		return;	
	else {
		handleExtDef(treeNode->childrenList[0]);
		handleExtDefList(treeNode->childrenList[1]);
	}
}	

void handleExtDef(GTN* treeNode) {
	//printf("start handle ExtDef\n");
	if(!strcmp(treeNode->childrenList[1]->unitName, "SEMI")) {
		handleSpecifier(treeNode->childrenList[0]);
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "ExtDecList")) {
		Type spectype = handleSpecifier(treeNode->childrenList[0]);
		handleExtDecList(treeNode->childrenList[1], spectype);
	}
	else if(!strcmp(treeNode->childrenList[1]->unitName, "FunDec")) {
		Type spectype = handleSpecifier(treeNode->childrenList[0]);
		if(!strcmp(treeNode->childrenList[2]->unitName, "CompSt")) {  // function definition
			handleFunDec(treeNode->childrenList[1], spectype, 1);
			handleCompSt(treeNode->childrenList[2], spectype);
		}
		else if(!strcmp(treeNode->childrenList[2]->unitName, "SEMI")) {  //function declaration
			handleFunDec(treeNode->childrenList[1], spectype, 0);
		}
			
	}
}

Type handleSpecifier(GTN* treeNode) {
	//printf("start handle Specifier\n");
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
		//printf("buffer field %s\n", buffer->u.structure.fieldlist->name);
		//printf("and buffer %s\n", buffer->u.structure.structname);
	}

	return buffer;

}

void handleExtDecList(GTN* treeNode, Type spectype) {
	//printf("start handle ExtDecList\n");
	if(treeNode->childrenNum == 1) {
		handleVarDec(treeNode->childrenList[0], spectype, NULL, 1);
	}
	else if(treeNode->childrenNum == 3) {
		handleVarDec(treeNode->childrenList[0], spectype, NULL, 1);
		handleExtDecList(treeNode->childrenList[2], spectype);
	}


}

void handleFunDec(GTN* treeNode, Type spectype, int isdefinition) {
	//printf("start handle FunDec\n");
	Type func = (Type) malloc(sizeof(Type_));
	func->kind = FUNCTION;
	func->u.function.funcname = treeNode->childrenList[0]->lexValue;
	func->u.function.paramnum = 0;
	func->u.function.rettype = spectype;
	func->u.function.decline = treeNode->childrenList[0]->lineNo;
	if(treeNode->childrenNum == 4) {
		func->u.function.param = handleVarList(treeNode->childrenList[2], 0);
		for(FieldList current = func->u.function.param; current != NULL; current = current->tail)
			func->u.function.paramnum ++;
	}

	if(isdefinition == 0) {    //declaration
		//printf("into dec\n");
		Sym funcSym = searchSymbol(treeNode->childrenList[0]->lexValue);
		if(funcSym == NULL) {    //not found
			func->u.function.isdefined = 0;
			func->u.function.param = handleVarList(treeNode->childrenList[2], 1);
			insertSymbol(treeNode->childrenList[0]->lexValue, func);
		}
		else {     //found
			//printf("check dec and dec\n");
			if( checktype(func->u.function.rettype, funcSym->type->u.function.rettype) == 0 )
				reportError(19, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
			else if( checkargs(func->u.function.param, funcSym->type->u.function.param) == 0) {
				reportError(19, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
			}
		}

	}
	else if(isdefinition == 1) {   //definition
		Sym funcSym = searchSymbol(treeNode->childrenList[0]->lexValue);
		if(funcSym == NULL) {    //not found
			func->u.function.isdefined = 1;
			//printf("first definition\n");
			func->u.function.param = handleVarList(treeNode->childrenList[2], 1);
			insertSymbol(treeNode->childrenList[0]->lexValue, func);
		}
		else {     //found
			//printf("found sym\n");
			if(funcSym->type->u.function.isdefined == 1) {
				reportError(4, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
			}
			else {
				if( checktype(func->u.function.rettype, funcSym->type->u.function.rettype) == 0 )
					reportError(19, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
				else if( checkargs(func->u.function.param, funcSym->type->u.function.param) == 0) 
					reportError(19, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
				else
					funcSym->type->u.function.isdefined = 1;
			}

		}

	}




	

}


FieldList handleVarList(GTN* treeNode, int add) {
	//printf("start handle VarList\n");
	//FieldList paramList = (FieldList) malloc(sizeof(FieldList_));
	if(treeNode->childrenNum == 1) {
		FieldList param = handleParamDec(treeNode->childrenList[0], add);
		return param;
	}
	else if(treeNode->childrenNum == 3){
		FieldList firstparam = handleParamDec(treeNode->childrenList[0], add);
		FieldList restparam = handleVarList(treeNode->childrenList[2], add);
		firstparam->tail = restparam;
		return firstparam;
	}
}

FieldList handleParamDec(GTN* treeNode, int add) {
	//printf("start handle ParamDec\n");
	FieldList param = (FieldList) malloc(sizeof(FieldList_));
	param->type = handleSpecifier(treeNode->childrenList[0]);
	//don't need a name here, param name is inserted into SymbolTable in 'handleVarDec'
	handleVarDec(treeNode->childrenList[1], param->type, NULL, add);
	param->tail = NULL;
	return param;
}

	
void handleCompSt(GTN* treeNode, Type rettype) {
	//printf("start handle CompSt\n");
	handleDefList(treeNode->childrenList[1], NULL);
	handleStmtList(treeNode->childrenList[2], rettype);
}

void handleStmtList(GTN* treeNode, Type rettype) {
	//printf("start handle StmtList\n");
	if(treeNode->childrenNum == 0)
		return;
	else if(treeNode->childrenNum == 2) {
		handleStmt(treeNode->childrenList[0], rettype);
		handleStmtList(treeNode->childrenList[1], rettype);
	}
}

void handleStmt(GTN* treeNode, Type rettype) {
	//printf("start handle Stmt and its childrennum is %d\n", treeNode->childrenNum);
	//printf("%s\n", treeNode->childrenList[0]->unitName);
	if(treeNode->childrenNum == 2) {
		handleExp(treeNode->childrenList[0]);
	}
	else if(treeNode->childrenNum == 1) {
		handleCompSt(treeNode->childrenList[0], rettype);
	}
	else if(treeNode->childrenNum == 3) {
		Type exptype = handleExp(treeNode->childrenList[1]);
		if(exptype == NULL || rettype == NULL)
			reportError(8, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
		else if(checktype(exptype, rettype) == 0)
			reportError(8, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
	} 
	else if(treeNode->childrenNum == 5 && !strcmp(treeNode->childrenList[0]->unitName, "IF")) {
		//printf("get in IF\n");
		Type condition = handleExp(treeNode->childrenList[2]);
		if(condition->kind != BASIC || condition->u.basic != 0) {
			reportError(7, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
		}
		handleStmt(treeNode->childrenList[4], rettype);
	} 
	else if(treeNode->childrenNum == 5 && !strcmp(treeNode->childrenList[0]->unitName, "WHILE")) {
		Type condition = handleExp(treeNode->childrenList[2]);
		if(condition->kind != BASIC || condition->u.basic != 0) {
			reportError(7, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
		}
		handleStmt(treeNode->childrenList[4], rettype);
	} 
	else if(treeNode->childrenNum == 7) {
		Type condition = handleExp(treeNode->childrenList[2]);
		if(condition->kind != BASIC || condition->u.basic != 0) {
			reportError(7, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
		}
		handleStmt(treeNode->childrenList[4], rettype);
		handleStmt(treeNode->childrenList[6], rettype);
	} 

}

Type handleExp(GTN* treeNode) {
	//printf("start handle Exp\n");
	if(treeNode->childrenNum == 3) {
		if(!strcmp(treeNode->childrenList[0]->unitName, "Exp"))
		{
			Type lefttype = handleExp(treeNode->childrenList[0]);
			if(lefttype == NULL)
				return NULL;
			if(!strcmp(treeNode->childrenList[1]->unitName, "DOT")) {
				if(lefttype->kind != STRUCTURE) {
					reportError(13, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else
				{
					char* name = treeNode->childrenList[2]->lexValue;
					//printf("name is %s\n", name);
					FieldList current = lefttype->u.structure.fieldlist;
					for(; current != NULL; current = current->tail) {
						if(!strcmp(current->name, name)) {
							return current->type;
						}
					}
					if(current == NULL) {
						reportError(14, treeNode->childrenList[2]->lineNo, treeNode->childrenList[2]->lexValue);
						return NULL;
					}
				}
			}


			Type righttype = handleExp(treeNode->childrenList[2]);
			if(righttype == NULL)
				return NULL;
			if(!strcmp(treeNode->childrenList[1]->unitName, "ASSIGNOP")) {
				//printf("found assign\n");
				//printf("%d\n", lefttype->kind);
				if(lefttype->kind == FUNCTION) {
					reportError(5, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				if(checktype(lefttype, righttype) == 0) {
					reportError(5, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				
				//printf("lef value is %s\n", treeNode->childrenList[0]->childrenList[0]->unitName);
				if(isleftvalue(treeNode->childrenList[0]) == 0) {
					reportError(6, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else {
					return lefttype;
				}
			}
			else if(!strcmp(treeNode->childrenList[1]->unitName, "AND") || !strcmp(treeNode->childrenList[1]->unitName, "OR")) {
				if(lefttype->kind != BASIC || righttype->kind != BASIC || lefttype->u.basic != 0 || righttype->u.basic != 0) {
					reportError(7, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else {
					return lefttype;
				}
			}
			
			
			else {
				if(lefttype->kind != BASIC || righttype->kind != BASIC || lefttype->u.basic != righttype->u.basic) {
					reportError(7, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else {
					return lefttype;
				}
			}
			
		}
		else
		{
			if(!strcmp(treeNode->childrenList[0]->unitName, "LP"))
			{
				return handleExp(treeNode->childrenList[1]);
			}
			else if(!strcmp(treeNode->childrenList[0]->unitName, "ID"))	{ // function without args
				Sym sym = searchSymbol(treeNode->childrenList[0]->lexValue);
				if(sym == NULL) {
					reportError(2, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else if(sym->type->kind != FUNCTION) {
					reportError(11, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else if(sym->type->u.function.paramnum != 0) {
					reportError(9, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else {
					return sym->type->u.function.rettype;
				}
			}
		}
		
		
	}
	else if(treeNode->childrenNum == 2) {
		Type mytype = handleExp(treeNode->childrenList[1]);
		if(mytype == NULL)
			return NULL;
		if(!strcmp(treeNode->childrenList[0]->unitName, "MINUS")) {
			if(mytype->kind != BASIC) {
				reportError(7, treeNode->childrenList[1]->lineNo, treeNode->childrenList[1]->lexValue);
				return NULL;
			}
			else {
				return mytype;
			}
		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "NOT")) {
			if(mytype->kind != BASIC || mytype->u.basic != 0) {
				reportError(7, treeNode->childrenList[1]->lineNo, treeNode->childrenList[1]->lexValue);
				return NULL;
			}
			else {
				return mytype;
			}
		}
		
	}
	else if(treeNode->childrenNum == 4) {
		if(!strcmp(treeNode->childrenList[0]->unitName, "Exp")) {
			Type array = handleExp(treeNode->childrenList[0]);
			if(array->kind != ARRAY) {
				reportError(10,treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->childrenList[0]->lexValue);
				return NULL;
			}

			Type index = handleExp(treeNode->childrenList[2]);
			if(index->kind != BASIC || index->u.basic != 0) {
				reportError(12,treeNode->childrenList[2]->lineNo, treeNode->childrenList[2]->childrenList[0]->lexValue);
				return NULL;
			}
			//printf("elem type %d\n", array->u.array.elem->kind);
			return array->u.array.elem;
			

		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {  //function with args
			Sym sym = searchSymbol(treeNode->childrenList[0]->lexValue);
				if(sym == NULL) {
					reportError(2, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else if(sym->type->kind != FUNCTION) {
					reportError(11, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
					return NULL;
				}
				else {
					FieldList arguments = handleArgs(treeNode->childrenList[2]);
					if(checkargs(sym->type->u.function.param, arguments) == 0) {
						reportError(9, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
						return NULL;
					}
				}
				
				return sym->type->u.function.rettype;
				
		}
		handleExp(treeNode->childrenList[0]);
	}
	else if(treeNode->childrenNum == 1) {
		if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {
			Sym sym = searchSymbol(treeNode->childrenList[0]->lexValue);
			if(sym == NULL) {
				reportError(1, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
				return NULL;
			}
			else {
				//printf("%s 's type is %d\n", sym->name, sym->type->kind);
				return sym->type;
			}
		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "INT")) {
			Type newtype = (Type) malloc(sizeof(Type_));
			newtype->kind = BASIC;
			newtype->u.basic = 0;
			return newtype;
		}
		else if(!strcmp(treeNode->childrenList[0]->unitName, "FLOAT")) {
			Type newtype = (Type) malloc(sizeof(Type_));
			newtype->kind = BASIC;
			newtype->u.basic = 1;
			return newtype;
		}
	}
}

int checktype(Type type1, Type type2) {
	if(type1 == NULL || type2 == NULL)
		return 0;
	if(type1->kind != type2->kind)
		return 0;
	if(type1->kind == BASIC) {
		if(type1->u.basic != type2->u.basic)
			return 0;
		else
			return 1;
	}
	else if(type1->kind == ARRAY) {
		Type cu1 = type1;
		Type cu2 = type2;
		//printf("array compare\n");
		//if(cu1->u.array.size != cu2->u.array.size)
			//return 0;
		if(checktype(cu1->u.array.elem, cu2->u.array.elem) == 1) {
			return 1;
		}
		else
			return 0;
	}
	else if(type1->kind == STRUCTURE) {
		//printf("compare struct name\n");

		//printf("%s\n",type1->u.structure.structname);
		if(strcmp(type1->u.structure.structname, type2->u.structure.structname) ) {
			//printf("compare end\n");
			return 0;
		}
		else
			return 1;
		
	}
	else if(type1->kind == FUNCTION) {
		if(strcmp(type1->u.function.funcname, type2->u.function.funcname) ) 
			return 0;
		else 
			return 1;
	}

}

int isleftvalue(GTN* treeNode) {
	//printf("%s\n", treeNode->childrenList[0]->unitName);
	//printf("call isleftvalue\n");
	if(!strcmp(treeNode->childrenList[0]->unitName, "ID")) {
		//printf("here\n");
		return 1;
	}
	else if(treeNode->childrenNum == 3  && !strcmp(treeNode->childrenList[1]->unitName, "DOT")) {
		//printf("here\n");
		return 1;
	}
	else if(treeNode->childrenNum == 3  &&  !strcmp(treeNode->childrenList[1]->unitName, "ASSIGNOP")) {
		//printf("here\n");
		return 1;
	}
	else if(treeNode->childrenNum == 4  && !strcmp(treeNode->childrenList[1]->unitName, "LB")) {
		return 1;
	}
	else if(!strcmp(treeNode->childrenList[0]->unitName, "LP")) {
		return isleftvalue(treeNode->childrenList[1]);
	}
	//printf("end isleftvalue\n");
	return 0;
}

FieldList handleArgs(GTN* treeNode) {
	FieldList newarg = (FieldList) malloc(sizeof(FieldList_));
	newarg->type = handleExp(treeNode->childrenList[0]);
	if(treeNode->childrenNum == 1) {
		newarg->tail = NULL;
	}
	else if(treeNode->childrenNum == 3) {
		newarg->tail = handleArgs(treeNode->childrenList[2]);
	}
	return newarg;
}

int checkargs(FieldList expected, FieldList get) {
	FieldList cu1 = expected;
	FieldList cu2 = get;
	while(cu1 != NULL && cu2 != NULL) {
		if(checktype(cu1->type, cu2->type) == 0)
			return 0;
		cu1 = cu1->tail; 
		cu2 = cu2->tail;
	}
	if( (cu1 == NULL && cu2 != NULL)  || (cu1 != NULL && cu2 == NULL) ) {
		return 0;
	}
	return 1;
}


void handleStructSpecifier(GTN* treeNode, Type buffer) {
	//printf("start handle StructSpecifier\n");
	if(treeNode->childrenNum == 2) {
		handleTag(treeNode->childrenList[1], buffer);
		//printf("in struct specifier is %s \n", buffer->u.structure.structname);
		//Sym structSym = searchSymbol("Temp2");
		//printf("in symtab temp2 is %s\n", structSym->type->u.structure.fieldlist->name);

	}
	else if(treeNode->childrenNum == 5) {
		handleOptTag(treeNode->childrenList[1], buffer);
		//printf("handle struct spec and stuct name is %s\n", buffer->u.structure.structname);
		
		//printf("a's %d %s\n", sym->type->kind, sym->type->u.structure.structname);
		handleDefList(treeNode->childrenList[3], buffer);
	}
}

void handleTag(GTN* treeNode, Type buffer) {
	//printf("start handle Tag\n");
	Sym structSym = searchSymbol(treeNode->childrenList[0]->lexValue);
	if(structSym == NULL) {
		reportError(17, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
	}
	else {
		buffer->u.structure.structname = structSym->type->u.structure.structname;
		buffer->u.structure.fieldlist = structSym->type->u.structure.fieldlist;
		//printf("new b is %s \n", buffer->u.structure.fieldlist->name);
	}
}

void handleOptTag(GTN* treeNode, Type buffer) {
	//printf("start handle OptTag\n");

	if(treeNode->childrenNum == 0) {
		return;
	}
	else if(treeNode->childrenNum == 1) {
		Sym structSym = searchSymbol(treeNode->childrenList[0]->lexValue);
		if(structSym == NULL) {
			//Type newtype = (Type)malloc(sizeof(Type_));
			buffer->u.structure.structname = treeNode->childrenList[0]->lexValue;
			//printf("new struct name here %s\n", buffer->u.structure.structname);
			insertSymbol(treeNode->childrenList[0]->lexValue, buffer);
			return;
		}
		else {
			reportError(16, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
			return;
		}
	}
}



void handleDefList(GTN* treeNode, Type buffer) {
	//printf("start handle DefList\n");
	if(treeNode->childrenNum == 0) 
		return;	
	else if(treeNode->childrenNum == 2){
		handleDef(treeNode->childrenList[0], buffer);
		handleDefList(treeNode->childrenList[1], buffer);
	}

}

void handleDef(GTN* treeNode, Type buffer) {
	//printf("start handle Def\n");
	Type spectype = handleSpecifier(treeNode->childrenList[0]);
	//printf("spec type in Def %s\n", spectype->u.structure.fieldlist->name);
	handleDecList(treeNode->childrenList[1], spectype, buffer);
	//printf("handle instructdef and stuct name is %s\n", buffer->u.structure.structname);
}


void handleDecList(GTN* treeNode, Type spectype, Type buffer) {
	//printf("start handle DecList\n");
	if(treeNode->childrenNum == 1) 
		handleDec(treeNode->childrenList[0], spectype, buffer);
	else if(treeNode->childrenNum == 3){
		handleDec(treeNode->childrenList[0], spectype, buffer);
		handleDecList(treeNode->childrenList[2], spectype, buffer);
	}

}

void handleDec(GTN* treeNode, Type spectype, Type buffer) {
	//printf("start handle Dec\n");
	handleVarDec(treeNode->childrenList[0], spectype, buffer, 1);
	if(treeNode->childrenNum == 3) {
		Type exptype = handleExp(treeNode->childrenList[2]);
		if(buffer != NULL)
			reportError(15, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->childrenList[0]->lexValue);
		else if(checktype(spectype, exptype) == 0) {
			reportError(5, treeNode->childrenList[0]->lineNo, treeNode->childrenList[0]->lexValue);
		}
	} 
 		
}

void handleVarDec(GTN* treeNode, Type spectype, Type buffer, int add) {
	//printf("start handle VarDec %s\n", treeNode->childrenList[0]->lexValue);
	if(treeNode->childrenNum == 1) {
		FieldList field_var = (FieldList)malloc(sizeof(FieldList_));
		field_var->name = treeNode->childrenList[0]->lexValue;
		field_var->type = spectype;
		field_var->tail = NULL;

		
		if(buffer != NULL)
		{	
			//printf("in struct\n");
			//printf("spec type %s\n", spectype->u.structure.structname);
			FieldList temp = buffer->u.structure.fieldlist;
			while(temp != NULL)
			{
				if(strcmp(temp->name, field_var->name) == 0)
				{
					reportError(15, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
					return ;
				}
				temp = temp->tail;
			}
			if(buffer->u.structure.fieldlist == NULL) {
				//printf("in structure \n");
				buffer->u.structure.fieldlist = field_var;
				//printf("fieldList name is %s\n", buffer->u.structure.fieldlist->name);
			}
				
			else
			{
				field_var->tail = buffer->u.structure.fieldlist;
				buffer->u.structure.fieldlist = field_var;
			}

			Sym check = searchSymbol(field_var->name);
			if(check != NULL) {
				reportError(3, treeNode->lineNo, treeNode->childrenList[0]->lexValue);
				return;
			}
			else {
				insertSymbol(field_var->name, field_var->type);
			}

		}
		else
		{
			if(add == 0)
				return;
			Sym sym = searchSymbol(treeNode->childrenList[0]->lexValue);
			if(sym == NULL) 
			{
				//printf("ready insert %s %d\n", field_var->name, field_var->type->kind);
				//printf("t de structname is %s\n", spectype->u.structure.fieldlist->name);
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
		handleVarDec(treeNode->childrenList[0], mytype, buffer, add);		
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
			printf("The left-hand side of an assignment must be a variable.\n");
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
		case 18:
			printf("Undefined function \"%s\".\n", msg);
			break;
		case 19:
			printf("Inconsistent declaration of function \"%s\".\n", msg);
			break;
	}
}



