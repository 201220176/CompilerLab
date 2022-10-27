%option yylineno

%{
#define YYSTYPE treeNode* 
#include "syntax.tab.h"
#include "tree.h"



int yycolumn = 1;
#define YY_USER_ACTION \
yylloc.first_line = yylloc.last_line = yylineno; \
yylloc.first_column = yycolumn; \
yylloc.last_column = yycolumn + yyleng - 1; \
yycolumn += yyleng;

extern int errorState;
%}

digit [0-9]
digits [0-9]+
floathead (\.[0-9]+|[0-9]+\.[0-9]*)
floatbody [+-]?[0-9]+
nofloathead ([0-9]+)|([0-9]*\.[0-9]*\.[0-9]*)+
nofloatbody (([0-9]*\.[0-9]*)|(\.)*)

%%
"int"|"float" {yylval = createTreeNode("TYPE",yytext);return TYPE;}
"struct" {yylval = createTreeNode("STRUCT",yytext);return STRUCT;}
"return" {yylval = createTreeNode("RETURN",yytext);return RETURN;}
"if"	{ yylval = createTreeNode("IF",yytext);return IF;}
"else"	{ yylval = createTreeNode("ELSE",yytext);return ELSE;}
"while"	{yylval = createTreeNode("WHILE",yytext);return WHILE;}
{digits}\.{digits} { yylval = createTreeNode("FLOAT",yytext);return FLOAT;}
{floathead}[eE]{floatbody} { yylval = createTreeNode("FLOAT",yytext);return FLOAT;}
[_a-zA-Z][_0-9a-zA-Z]* {yylval = createTreeNode("ID",yytext);return ID;}
({nofloathead}[eE]({nofloatbody}|{floatbody}))|({floathead}[eE]{nofloatbody}) { 	
	errorState = 1;
	printf("Error type A at Line %d: Illegal float \'%s\'\n",yylineno, yytext);
	yylval = createTreeNode("FLOAT",yytext);return FLOAT;}
0|[1-9]{digit}*  {yylval = createTreeNode("INT",yytext);return INT;}
0[0-7]+ {yylval = createTreeNode("INT",yytext);return INT;}
0[xX][0-9a-fA-F]+ {yylval = createTreeNode("INT",yytext);return INT;}
0[xX][0-9a-fA-F]+[g-zG-Z]+[0-9a-fA-F]* {
	errorState = 1;
	printf("Error type A at Line %d: Illegal hexadecimal number \'%s\'\n",yylineno, yytext);
	yylval = createTreeNode("INT",yytext);return INT;
}
0[0-7]*[8-9]+[0-7]* {
	errorState = 1;
	printf("Error type A at Line %d: Illegal octal number \'%s\'\n",yylineno, yytext);
	yylval = createTreeNode("INT",yytext);return INT;
}
[1-9][0-9]*[a-zA-Z]+[0-9]* {
	errorState = 1;
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);
	yylval = createTreeNode("INT",yytext);return INT;
}
";" {yylval = createTreeNode("SEMI",yytext);return SEMI;}
"," {yylval = createTreeNode("COMMA",yytext);return COMMA;}
"=" {yylval = createTreeNode("ASSIGNOP",yytext);return ASSIGNOP;}
>=|<=|==|!=|>|< {yylval = createTreeNode("RELOP",yytext);return RELOP;}
"+" {  yylval = createTreeNode("PLUS",yytext);return PLUS;}
"-" { yylval = createTreeNode("MINUS",yytext);return MINUS;}
"*" {  yylval = createTreeNode("STAR",yytext);return STAR;}
"/" { yylval = createTreeNode("DIV",yytext);return DIV;}
"&&" {yylval = createTreeNode("AND",yytext);return AND;}
"||" {yylval = createTreeNode("OR",yytext);return OR;}
"." {yylval = createTreeNode("DOT",yytext);return DOT;}
"!" { yylval = createTreeNode("NOT",yytext);return NOT;}
"(" { yylval = createTreeNode("LP",yytext);return LP;}
")" { yylval = createTreeNode("RP",yytext);return RP;}
"[" { yylval = createTreeNode("LB",yytext);return LB;}
"]" {yylval = createTreeNode("RB",yytext);return RB;}
"{" {yylval = createTreeNode("LC",yytext);return LC;}
"}" { yylval = createTreeNode("RC",yytext);return RC; }
"//" {   char c=input(); while(c!='\n')	c=input(); }
"/*"  {
            int line=yylineno;
            char c;
			int state = 0;
			while(c=input())
			{
				switch(state){
					case 0 :
						if(c=='*')	
							state = 1;
						break;
					case 1:
						if(c=='/')
							state = 2;
						else
							state = 0;
						break;
					default: break;
				}
				if (state == 2)	break;
			}
			if(state!=2){
                errorState=1;
                printf("Error Type B at Line %d: The '\\*' can not find a matched '*/'.\n",line);
            }
        }  
"*/" {
			errorState=1;
			printf("Error Type B at Line %d: syntax error,not-matched '*/'.\n", yylineno);
            }

" "|\t  {}
[\n\r] {yycolumn = 1;}
. {
	errorState = 1;
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);
}

%%