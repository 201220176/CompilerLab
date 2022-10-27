%locations
%{
#include <stdio.h>
#include "tree.h"
#include "lex.yy.c"
extern void  yyerror(const char *msg,...);
void yyrestart(FILE *yyin);
int yydebug=1;
int errorState = 0;
treeNode *root;
%}
%token INT FLOAT ID
%token SEMI COMMA
%token TYPE
%token STRUCT RETURN IF ELSE WHILE
%token LC RC

%right ASSIGNOP
%left OR 
%left AND
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT 
%left LP RP LB RB DOT	

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%

Program : ExtDefList { 
	$$= createTreeNode2("Program", 1, $1);
	root = $$; 
}
;
ExtDefList : {$$ = NULL;}
|ExtDef ExtDefList {$$= createTreeNode2("ExtDefList", 2, $1, $2);}
;
//表示一个全局变量、结构体或函数的定义
ExtDef : Specifier ExtDecList SEMI {$$= createTreeNode2("ExtDef", 3, $1, $2, $3);}
| Specifier SEMI {$$= createTreeNode2("ExtDef", 2, $1, $2);}
| Specifier FunDec CompSt {$$= createTreeNode2("ExtDef", 3, $1, $2, $3);}
| Specifier FunDec SEMI {$$= createTreeNode2("ExtDef", 3, $1, $2, $3);}
//类型错误
| error ExtDecList SEMI {yyerror("Specifier Illegal in ExtDef",1,@1.first_line);}
| error	SEMI {yyerror("Specifier Illegal in ExtDef",1,@1.first_line);}
| error FunDec CompSt {yyerror("cannot Define in ExtDef",1,@1.first_line);}
//函数头错误
| Specifier error CompSt  {yyerror("cannot Define in ExtDef",1,@2.first_line);}
//缺少分号
| Specifier ExtDecList {yyerror("Missing ';' in ExtDef",1,@2.first_line);}
| Specifier {yyerror("Missing ';' in ExtDef",1,@1.first_line);}
;

ExtDecList : VarDec {$$= createTreeNode2("ExtDecList", 1, $1);}
| VarDec COMMA ExtDecList {$$= createTreeNode2("ExtDecList", 3, $1, $2, $3);}
;

//变量的类型
Specifier : TYPE {$$= createTreeNode2("Specifier", 1, $1);}
| StructSpecifier {$$= createTreeNode2( "Specifier", 1, $1);}
;
StructSpecifier : STRUCT OptTag LC DefList RC {$$= createTreeNode2("StructSpecifier", 5, $1, $2, $3, $4, $5);}
| STRUCT Tag {$$= createTreeNode2("StructSpecifier", 2, $1, $2);}
|  STRUCT  error RC {yyerror("something wrong in StructSpecifier",1,@2.first_line);}
|  error OptTag LC DefList RC {yyerror("something wrong in StructSpecifier,1,@1.first_line");}
;
OptTag : {$$ = NULL;}
|ID {$$= createTreeNode2("OptTag", 1, $1);}
;
Tag : ID {$$= createTreeNode2("Tag", 1, $1);}
;

//变量的定义，形式为 name[INT][INT]
VarDec : ID {$$= createTreeNode2("VarDec", 1, $1);}
| VarDec LB INT RB {$$= createTreeNode2("VarDec", 4, $1, $2, $3, $4);}
| VarDec LB error RB {yyerror("Missing ']' in VarDec",1,@3.first_line);}
;

//函数头的定义
FunDec : ID LP VarList RP {$$= createTreeNode2("FunDec", 4, $1, $2, $3, $4);}
| ID LP RP {$$= createTreeNode2("FunDec", 3, $1, $2, $3);}
|  ID LP error RP  {yyerror("something wrong in FunDec",1,@3.first_line);}
| ID error RP  {yyerror("something wrong in FunDec",1,@2.first_line);}
;
VarList : ParamDec COMMA VarList  {$$= createTreeNode2("VarList", 3, $1, $2, $3);}
| ParamDec {$$= createTreeNode2("VarList", 1, $1);}
;
ParamDec : Specifier VarDec {$$= createTreeNode2("ParamDec", 2, $1, $2);}
;

//花括号括起来的语句块
CompSt : LC DefList StmtList RC {$$= createTreeNode2("CompSt", 4, $1, $2, $3, $4);}
;
StmtList : {$$ = NULL;}
|Stmt StmtList {$$= createTreeNode2("StmtList", 2, $1, $2);}
;

//每个Stmt都表示一条语句
Stmt : Exp SEMI {$$= createTreeNode2("Stmt", 2, $1, $2);}
| CompSt {$$= createTreeNode2("Stmt", 1, $1);}
| RETURN Exp SEMI {$$= createTreeNode2("Stmt", 3, $1, $2, $3);}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$= createTreeNode2("Stmt", 5, $1, $2, $3, $4, $5);}
| IF LP Exp RP Stmt ELSE Stmt {$$= createTreeNode2("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
|  IF error ELSE Stmt  {yyerror("something wrong in Stmt",1,@2.first_line);}
| WHILE LP Exp RP Stmt {$$= createTreeNode2("Stmt", 5, $1, $2, $3, $4, $5);}
|  WHILE error RP {yyerror("something wrong in Stmt",1,@2.first_line);}
|  WHILE error RC {yyerror("something wrong in Stmt",1,@2.first_line);}
|  error SEMI  {yyerror("something wrong in Stmt",1,@1.first_line);}
|  Exp  {yyerror("something wrong in Stmt",1,@1.first_line);}
|  RETURN Exp   {yyerror("something wrong in Stmt",1,@2.first_line);}
;
DefList : {$$ = NULL;}
|Def DefList {$$= createTreeNode2("DefList", 2, $1, $2);}
;
//一条变量定义
Def : Specifier DecList SEMI {$$= createTreeNode2("Def", 3, $1, $2, $3);}
|  Specifier error SEMI  {yyerror("something wrong in Def",1,@2.first_line);}
|  error DecList SEMI  {yyerror("something wrong in Def",1,@1.first_line);}
|  Specifier DecList {yyerror("something wrong in Def",1,@2.first_line);}
;
DecList : Dec {$$= createTreeNode2("DecList", 1, $1);}
| Dec COMMA DecList {$$= createTreeNode2("DecList", 3, $1, $2, $3);}
;
Dec : VarDec {$$= createTreeNode2("Dec", 1, $1);}
| VarDec ASSIGNOP Exp {$$= createTreeNode2( "Dec", 3, $1, $2, $3);}
;

//表达式
Exp : Exp ASSIGNOP Exp {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| Exp AND Exp {$$= createTreeNode2( "Exp", 3, $1, $2, $3);}
| Exp OR Exp {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| Exp RELOP Exp {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| Exp PLUS Exp {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| Exp MINUS Exp {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| Exp STAR Exp {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| Exp DIV Exp {$$= createTreeNode2( "Exp", 3, $1, $2, $3);}
| LP Exp RP {$$= createTreeNode2( "Exp", 3, $1, $2, $3);}
| MINUS Exp {$$= createTreeNode2( "Exp", 2, $1, $2);}
| NOT Exp {$$= createTreeNode2("Exp", 2, $1, $2);}
| ID LP Args RP {$$= createTreeNode2("Exp", 4, $1, $2, $3, $4);}
| ID LP RP {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| Exp LB Exp RB {$$= createTreeNode2("Exp", 4, $1, $2, $3, $4);}
| Exp DOT ID {$$= createTreeNode2("Exp", 3, $1, $2, $3);}
| ID {$$= createTreeNode2( "Exp", 1, $1);}
| INT {$$= createTreeNode2("Exp", 1, $1);}
| FLOAT {$$= createTreeNode2("Exp", 1, $1);}
;

Args : Exp COMMA Args {$$= createTreeNode2("Args", 3, $1, $2, $3);}
| Exp {$$= createTreeNode2("Args", 1, $1);}
;
%%
//#include "lex.yy.c"
void yyerror(const char* msg,...) {
	errorState = 1;
	va_list args;
	va_start(args,msg);
	int argnums = va_arg(args, int);
	if(strcmp(msg,"syntax error"))
	{
	int line = va_arg(args, int);
	fprintf(stderr,"Error type B at Line %d: %s.\n", line, msg);
	}
	//else
	//	fprintf(stderr,"Error type B at Line %d: %s.\n", yylloc.first_line, msg);
	va_end(args);
}