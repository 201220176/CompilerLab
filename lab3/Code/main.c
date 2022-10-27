#include "tree.h"
#include "syntax.tab.h"
#include"semantic.h"
#include"IR.h"
extern FILE* yyin;
//extern int yylex();
extern int errorState;
extern void yyrestart(FILE* yyin);
extern treeNode *root;
extern int yydebug;
int main(int argc, char** argv)
{
	if(argc <= 1) return 1;

	FILE* f = fopen(argv[1], "r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}


	char temp[1024];
	char *buf=(char *)malloc(1024) ;
	int len = 0,alllen=0;
	/*
	while(fgets(temp,1024,f) != NULL)
 	{
 	len = strlen(temp);
	alllen+=(len-1);
	temp[len-1] = '\0';  
	for(int i =0 ;i<len-1;++i)
	{
		(*buf)=temp[i];
		++buf;
	}
	 }
	buf-=alllen;
	printf("%s",buf);
	rewind(f);
*/
/*
	while(fgets(temp,1024,f) != NULL)
 	{
	
 	len = strlen(temp);
	temp[len-1] = '\0';  	
	printf("%s\n",temp);
	}
	rewind(f);
*/
	yyrestart(f);
	yydebug = 1;
	yyparse();
	if(!errorState)
	{
		//PrintTree(root,0);
		tableInit();
        Program(root);
		printTable();
		//Translate(root);
	}

	return 0;
}