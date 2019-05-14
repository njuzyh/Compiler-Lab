#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "grammartree.h"

GTN* newLexNode(char* unitName, int lineNo, char* lexValue)
{
	GTN* buffer = (GTN*)malloc(sizeof(GTN));
	/*GTN new_tree_node;
	new_tree_node.unitName = unitName;
	new_tree_node.lineNo = lineNo;
	new_tree_node.lex = 1;
	new_tree_node.lexValue = lexValue;
	new_tree_node.childrenNum = 0;
	new_tree_node.childrenList = NULL;
	buffer = &new_tree_node;*/
	buffer->unitName = unitName;
	buffer->lineNo = lineNo;
	buffer->lex = 1;
	char *s;
	s = (char*)malloc(sizeof(char*) * 20);
	strcpy(s, lexValue);
	buffer->lexValue = s;
	buffer->childrenNum = 0;
	//buffer->childrenList = NULL;
	return buffer;
}

GTN* newGramNode(char* unitName, int childrenNum, ...)
{
	GTN* buffer = (GTN*)malloc(sizeof(GTN));
	/*GTN new_tree_node;
	new_tree_node.unitName = unitName;
	new_tree_node.lex = 0;
	new_tree_node.lexValue = "";
	new_tree_node.childrenNum = childrenNum;*/
	buffer->unitName = unitName;
	buffer->lex = 0;
	buffer->lexValue = "";
	buffer->childrenNum = childrenNum;

	if(childrenNum == 0) {
		//buffer->childrenList = NULL;
		//buffer = &new_tree_node;
		return buffer;
	}
	//buffer->childrenList = malloc(sizeof(GTN*) * childrenNum);
	va_list arg_ptr;
	GTN* current;
	//GTN** temp = new_tree_node.childrenList;
	va_start(arg_ptr, childrenNum);
	current = va_arg(arg_ptr, GTN*);
	buffer->lineNo = current->lineNo;
	buffer->childrenList[0] = current;
	int count = 1;
	while(count < childrenNum) {	
		//temp++;
		current = va_arg(arg_ptr, GTN*);
		count ++;
		buffer->childrenList[count - 1] = current;
	}
	va_end(arg_ptr);

	//buffer = &new_tree_node;
	return buffer;

}

void printTabSpace(int tabspaceNum)
{
	for(int i = 0; i < tabspaceNum; i++)
		printf("  ");
}


void outputNode(GTN* gramNode, int tabspace)
{	
	//printf("output\n");
	//printf("gramNode is %x\n", gramNode);
	if(gramNode->lex == 1) {
		printTabSpace(tabspace);
		printf("%s", gramNode->unitName);
		if(!strcmp(gramNode->unitName, "TYPE") || !strcmp(gramNode->unitName, "ID") || !strcmp(gramNode->unitName, "INT") || !strcmp(gramNode->unitName, "FLOAT"))
		printf(": %s", gramNode->lexValue);
		printf("\n");
	}
	else {
		if(gramNode->childrenNum == 0)
			return;
		printTabSpace(tabspace);
		printf("%s(%d)\n", gramNode->unitName, gramNode->lineNo);
		//GTN** temp = gramNode->childrenList;
		for(int i = 0; i < gramNode->childrenNum; i++) {
			outputNode(gramNode->childrenList[i], tabspace + 1);
			//temp++;
		}
	}
}
