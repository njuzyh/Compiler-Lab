#include <stdio.h>
#include "grammartree.h"
#include "symbol.h"
#define SymTabSize 16384
extern FILE* yyin;
extern void yyrestart  (FILE * input_file );
extern int yyparse(void);
int wrong = 0;
GTN* programRoot;
Sym symbolTable[SymTabSize];

int main(int argc, char** argv) {
	if(argc <= 1)
		return 1;
	FILE* f = fopen(argv[1], "r");
	if(!f) {
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	init_symbolTable();
	//outputNode(programRoot, 0);
	startProgram(programRoot);
	//printSymTab();
	
	
	//printf("find g and param num is %d, %d\n", sym->type->u.function.paramnum, sym->type->u.function.param->type->u.basic);
	//printf("%d %d", sym->type->u.array.size, sym->type->u.array.elem->u.array.size);
	//printf("a's %d %s\n", sym->type->kind, sym->type->u.structure.structname);
	
	return 0;
}
