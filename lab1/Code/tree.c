#include "tree.h"
extern int yylineno;

int s2Int(const char *s){
	if(!strcmp(s,"0"))
		return 0;
	if(s[0] == '0'&&(s[1] == 'x'|| s[1] == 'X'))
			return strtol(s,NULL,16);
	else if(s[0] == '0')
		return strtol(s,NULL,8);
	else 
		return atoi(s);
}
float s2Float(const char *s){
	return strtof(s,NULL);
}
char *s2Str(const char *s){
	char * tmp = (char *)malloc(sizeof(s));
	strcpy(tmp, s);
	return tmp;
}

treeNode* createTreeNode(const char* name,const char* value)
{
    treeNode* temp =(treeNode*) malloc (sizeof(treeNode));
    temp->name = s2Str(name);
    temp->line = yylineno;
	temp->type=Terminal_symbol;
    if(!strcmp(name, "INT")) 
		temp->i_val = s2Int(value); 
    else if(!strcmp(name, "FLOAT")) 
		temp->f_val = s2Float(value); 
    else 
		temp->s_val = s2Str(value); 
    return temp;
}

treeNode* createTreeNode2(const char* name,int num, ...)
{
    treeNode* res =(treeNode*) malloc (sizeof(treeNode));
	va_list args;
	va_start(args,num);

	treeNode *l_child;
	res->name = s2Str(name);
	res->type=non_Terminal;
	l_child = va_arg(args, treeNode*);
	res->line = l_child->line;
	res->l_child=l_child;
	for(int i = 1 ; i < num ; ++i)
	{
		l_child = va_arg(args, treeNode*);
		treeNode *temp = res->l_child;
		while(temp->r_child != NULL)
			temp = temp->r_child;
		temp->r_child = l_child;
	}
	va_end(args);
    return res;
}

void PrintTree(treeNode *root,int depth){
	if(root != NULL)
	{
		for(int i = 0; i < depth; i++)
			printf("  ");
		printf("%s",root->name);
		if(root->type == non_Terminal)
			printf(" (%d)", root->line);
		else if(root->type == Terminal_symbol)
		{
			if(!strcmp(root->name, "ID") || !strcmp(root->name,"TYPE"))
				printf(": %s",root->s_val);
			else if(!strcmp(root->name, "INT"))
				printf(": %d",root->i_val);
			else if(!strcmp(root->name, "FLOAT"))
				printf(": %f",root->f_val);
		}
		printf("\n");
		PrintTree(root->l_child,depth+1);
		PrintTree(root->r_child,depth);
	}
}