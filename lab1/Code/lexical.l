%option yylineno
%{

%}
digit  [0-9]
digits [0-9]+
letter  [a-zA-Z]
RELOP  >|<|>=|<=|==|!=
     
%%

0|[1-9]{digit}*  {return INT;}
{digits}\.{digits}  {return FLOAT;}
"int"|"float" {return TYPE;}
{RELOP}   {return RELOP;}
";"   {return SEMI;}
","   {return COMMA;}
"="   {return ASSIGNOP;}
"+"   {return PLUS;}
"-"   {return MINUS;}
"*"   {return STAR;}
"/"   {return DIV;}
"&&"    {return AND;}
"||"   {return OR;}
"."   {return DOT;}
"!"   {return NOT;}
"("   {return LP;}
")"   {return RP;}
"["   {return LB;}
"]"   {return RB;}
"{"   {return LC;}
"}"   {return RC;}
"struct"   {return STRUCT;}
"return"   {return RETURN;}
"if"   {return IF;}
"else"   {return ELSE;}
"while"   {return WHILE;}
" "|\t  {;}
[_a-zA-Z][_0-9a-zA-Z]*  { return ID;}
.   {
    printf("Error type A at Line %d: Mysterious character \'%s'\n",yylineno,yytext);
}

   
    

%%
    /*代码部分*/