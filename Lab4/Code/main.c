#include <stdio.h>
#include "grammartree.h"
#include "symbol.h"
#include "intermediate.h"
#include "spim.h"
#define SymTabSize 16384
extern FILE* yyin;
extern void yyrestart  (FILE * input_file );
extern int yyparse(void);
int wrong = 0;
GTN* programRoot;
Sym symbolTable[SymTabSize];
InterCodes intercodes = NULL;

int main(int argc, char** argv) {
	if(argc <= 1)
		return 1;
	FILE* f = fopen(argv[1], "r");
	FILE* fout = NULL;
	if(!f) {
		perror(argv[1]);
		return 1;
	}

	if(argc == 3) {
		yyrestart(f);
		yyparse();
		init_symbolTable();
		handleProgram(programRoot);
		intercodes = translateProgram(programRoot);
		initRegisters();
		//printTestCodes(intercodes);
		fout = freopen(argv[2], "w", stdout);
		printMIPS(intercodes);
		fclose(fout);
	}
	

	
	return 0;
}
