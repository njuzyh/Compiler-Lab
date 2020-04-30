extern int yylineno;
extern char* yytext;

#ifndef _GRAMMARTREE_H_
#define _GRAMMARTREE_H_

typedef struct GrammarTreeNode {
	char* unitName;
	int lineNo;
	int lex;
	char* lexValue;
	int childrenNum;
	struct GrammarTreeNode* childrenList[10];
}GTN;


GTN* newLexNode(char* unitName, int lineNo, char* lexValue);

GTN* newGramNode(char* unitName, int childrenNum, ...);

void outputNode(GTN* gramNode, int tabspace);

void printTabSpace(int tabspaceNum);

#endif
