#ifndef TREE_H_
#define TREE_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


typedef struct treeNode {
	int line;
	char *name;
	int type;
	union{
		int i_val;
		float f_val;
		char *s_val;
	};
	struct treeNode *l_child, *r_child;
}treeNode;

enum{
    Terminal_symbol,non_Terminal
} ;

int s2Int(const char *);
float s2Float(const char *);
char *s2Str(const char *);
treeNode* createTreeNode(const char* name,const char* value);
treeNode* createTreeNode2(const char* name,int num,...);
void PrintTree(treeNode *,int);
#endif