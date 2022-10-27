#include"IR.h"

void translate_init()
{

}

void translate(treeNode *root)
{
    if(root == NULL)
        return;
    translate_init();
    translate_Program(root);

}

void translate_Program(treeNode *node)
{
    if (node==NULL)
        return;
    if (node->child!=NULL) 
    {
        translate_ExtDefList(node->child);
    }
}

void translate_ExtDefList(treeNode* node)
{
        if (node==NULL)
             return;
        translate_ExtDef(node->child);
        translate_ExtDefList(node->child->bro);
}

void translate_ExtDef(treeNode* node)
{
    if (node==NULL)
             return;
     // Specifier ExtDecList SEMI
     if(!strcmp("ExtDecList",node->child->bro->name))
     {
        translate_ExtDecList(node->child->bro);
    }
     else if(!strcmp("FunDec",node->child->bro->name))
     {
        // Specifier FunDec CompSt
        if(!strcmp("CompSt",node->child->bro->bro->name))
            {
                translate_FunDec(node->child->bro);
                translate_CompSt(node->child->bro->bro);
            }
        //Specifier FunDec SEMI
        else
            translate_FunDec(node->child->bro);
     }
    // Specifier SEMI
    else
    {
    }
}

void translate_CompSt(treeNode* node) 
{	 
    if (node==NULL)
             return ;
        //CompSt -> LC DefList StmtList RC
		if(!strcmp("DefList",node->child->bro->name)) 
		{
			translate_DefList(node->child->bro);
			if(!strcmp("StmtList",node->child->bro->bro->name))
				translate_StmtList(node->child->bro->bro);
		}
        //CompSt -> LC StmtList RC
		else if(!strcmp("StmtList",node->child->bro->name))
				translate_StmtList(node->child->bro);
		
}

void translate_StmtList(treeNode* node)
{
    if (node==NULL)
             return ;
		if(node->child) 
		{
            //StmtList -> Stmt StmtList
			if(!strcmp("Stmt",node->child->name)) 
			{
				translate_Stmt(node->child);
				translate_StmtList(node->child->bro);
			}
			else{
			//StmtList -> empty
			}
		}
}

void translate_Stmt(treeNode* node)
{
    if (node==NULL)
             return ;
		if(node->child)
		{ 
            //Stmt -> Exp SEMI
			if(!strcmp("Exp",node->child->name ))
			{

				translate_Exp(node->child);
			}
            //Stmt -> CompSt
			else if(!strcmp("CompSt",node->child->name )) 
			{
				translate_CompSt(node->child);
			}
            //Stmt -> RETURN Exp SEMI
			else if(!strcmp( "RETURN",node->child->name)) 
			{

			}
            //Stmt -> IF LP Exp RP Stmt | IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt
			else if(!strcmp("IF",node->child->name) || !strcmp("WHILE",node->child->name )) 
			{
				translate_Exp(node->child->bro->bro);
				translate_Stmt(node->child->bro->bro->bro->bro);
                //Stmt ->IF LP Exp RP Stmt ELSE Stmt 
				if(node->child->bro->bro->bro->bro->bro!=NULL)  
				{
					translate_Stmt(node->child->bro->bro->bro->bro->bro->bro);
				}
			}
		}
}

Type* translate_Exp(treeNode* node)
{
    if (node==NULL)
            return NULL;
    Type* type = NULL;
    //Exp->INT
     if(!strcmp("INT",node->child->name))
        {
            type =(Type*)malloc(sizeof(Type));
            type->kind = BASIC;
            type->u.basic = 0;
            return type;
        }
    //Exp->FLOAT
    else if(!strcmp("FLOAT",node->child->name))
        {
            type =(Type*)malloc(sizeof(Type));
            type->kind = BASIC;
            type->u.basic = 1;
            return type;
        }
    else if(!strcmp("Exp",node->child->name))
        {
            Type* type1 =  translate_Exp(node->child);
            if(type1==NULL)
                return NULL;
            if(node->child->bro==NULL)
                return NULL;
            //Exp -> Exp ASSIGNOP Exp
            if(!strcmp("ASSIGNOP",node->child->bro->name))
            {
                //检查第一个Exp是否为左值
                int isLvalue = 0;
                treeNode* E1 = node->child;
                //E1->ID 
                if(E1->child&&!strcmp("ID",E1->child->name)&&!E1->child->bro)
                    isLvalue=1;
                else if(E1->child&&!strcmp("Exp",E1->child->name))
                {
                    // E1->Exp LB Exp RB E1->Exp DOT ID
                    if(E1->child->bro&&!strcmp("LB",E1->child->bro->name))
                        isLvalue=1;
                    else if(E1->child->bro&&!strcmp("DOT",E1->child->bro->name))
                        isLvalue=1;
                }
                if(!isLvalue)
                    serror(Type6,E1->line,"The left-hand side of an assignment must be a variable");
                Type *type2 =  translate_Exp(node->child->bro->bro);
                if(!BeSameType(type1,type2))
                    serror(Type5,E1->line,"The mismatched for assignment");
                return type1;
            }
            //Exp -> Exp LB Exp RB
            else if(!strcmp("LB",node->child->bro->name))
            {
                Type* para =  translate_Exp(node->child->bro->bro);
                if(type1->kind!=ARRAY)
                    serror(Type10,node->child->line,"wrong operation on non-array type");
                else if(para->kind!=BASIC||para->u.basic!=0)
                    serror(Type12,node->child->bro->bro->line,"a non-integer given to array");
                else type=type1->u.array.elem;
                return type;
            }
            //Exp -> Exp DOT ID
            else if(!strcmp("DOT",node->child->bro->name))
            {
                Type *type1 = Exp(node->child);
                //不是结构体，或者只是名字。
                if(type1&&(type1->kind!=STRUCTURE||type1->is_var==0))
                {
                    serror(Type13,node->child->line,"use '.' on a non-structure");
                    return NULL;
                }
                //查看ID是否在结构体中
                else if(type1&&type1->kind==STRUCTURE&&!isDoInStructure(node->child->bro->bro->s_val,type1))
                {
                    serror(Type14,node->child->line,"use an undefined domain in a structure");
                    return NULL;
                }
                //语句合法
                else if(type1&&type1->kind==STRUCTURE&&isDoInStructure(node->child->bro->bro->s_val,type1))
                {
                    type = getTypeFromStruct(node->child->bro->bro->s_val,type1);
                    return type;
                }
            }
            //Exp -> Exp AND/OR/RELOP/PLUS/MINUS/STAR/DIV EXP
            else 
            {
                Type* type2 =  translate_Exp(node->child->bro->bro);
                if(!checkOperator(type1,type2,node->child->bro->name))
                {
                      serror(Type7,node->child->line,"Type mismatched for operands");
                }
                else 
                {
                    if(!strcmp("RELOP",node->child->bro->name)||!strcmp("AND",node->child->bro->name)||!strcmp("OR",node->child->bro->name))
                        {
                            type=(Type*)malloc(sizeof(Type));
                            type->kind=BASIC;
                            type->u.basic=0;
                        }
                    else
                        type = type1;
                }
                return type;
            }
        }
        //EXP-> LP Exp RP | MINUS EXP
        else if(!strcmp("LP",node->child->name)||!strcmp("MINUS",node->child->name))
            {
                type= translate_Exp(node->child->bro);
                return type;
            } 
        //EXP-> NOT EXP
        else if(!strcmp("NOT",node->child->name))
            {
                type= translate_Exp(node->child->bro);
                if(type->kind!=BASIC)
                    serror(Type7,node->child->line,"Type mismatched for operands");
                return type;
            } 
        //EXP-> ID | ID LP RP | ID LP Args RP
        else if(!strcmp("ID",node->child->name))
            {
                //EXP-> ID
                if(node->child->bro==NULL)
                {
                    type = getTypeFromTable(node->child->s_val);
                    if(type == NULL)
                        serror(Type1,node->child->line,"Undefined variable");
                    return type;
                }
                //EXP-> ID LP RP | ID LP Args RP
                else
                {
                    type = getTypeFromTable(node->child->s_val);
                    Type* res = NULL;
                    int argflag=0;          //是否需要args
                    if(type == NULL)
                        serror(Type2,node->child->line,"Undefined function");
                    else if(type->kind!=FUNCTION)
                        serror(Type11,node->child->line,"function operation on non-function");
                    else
                    {
                        argflag=type->u.function.para==NULL?0:1;
                        res = type->u.function.ret;
                    }
                    if(!strcmp("Args",node->child->bro->bro->name))
                        translate_Args(node->child->bro->bro);
                    else if(argflag==1)
                         serror(Type9,node->child->line,"function need arguments");
                    return res;
                }
            }
}

void  translate_Args(treeNode* node)
{
    if(node==NULL)
        return;
    treeNode*cur = node;
    while(cur->child)
    {
        Type* temp = translate_Exp(cur->child);
        if(cur->child->bro)
        {
            cur = cur->child->bro->bro;
        }
        else
            break;
    }
}

void translate_FunDec(treeNode* node)
{
    if (node==NULL)
             return;
        //FunDec -> ID LP VarList RP
        if(!strcmp( "VarList",node->child->bro->bro->name))   
        {
            translate_VarList(node->child->bro->bro);
        }
        //FunDec -> ID LP RP
        else    
        {

        }

}


void  translate_VarList(treeNode* node)
{
        if (node==NULL)
             return;
        // VarList -> ParamDec COMMA VarList
        if(node->child->bro)  
        {
             translate_ParamDec(node->child);
            translate_VarList(node->child->bro->bro);
        }
        //VarList -> ParamDec
        else    
        {
            translate_ParamDec(node->child);
        }
}

void translate_ParamDec(treeNode* node)
{
        if (node==NULL)
             return;
        FieldList* temp = (FieldList*)malloc(sizeof(FieldList));
        //Specifier VarDec
        translate_VarDec(node->child->bro);
}

void translate_ExtDecList(treeNode* node)
{
    if (node==NULL)
        return;
    //ExtDecList -> VarDec COMMA ExtDecList
    if(node->child->bro!=NULL) 
    {
        translate_VarDec(node->child); 
        translate_ExtDecList(node->child->bro->bro);
    }
    //ExtDecList -> VarDec
    else 
    { 
        translate_VarDec(node->child);
    }
}

//name[INT][INT]，返回name的类型
Type* translate_VarDec(treeNode* node)
{
    if (node==NULL)
        return NULL;
        //VarDec -> ID
        if(!strcmp("ID",node->child->name))
        {
            
        }
        //VarDec -> VarDec LP INT RP
        else  
        {	
            return translate_VarDec(node->child);
        }
}

void translate_DefList(treeNode* node)
{
    if (node==NULL)
        return ;
     // DefList-> Def DefList    
    if(node->child)
    {
        translate_Def(node->child);
        translate_DefList(node->child->bro);              
    }
    // DefList-> empty
    else
    {
        return;
    }
}

void translate_Def(treeNode* node)
{
    if (node==NULL)
        return;
    //Def -> Specifier DecList SEMI
    if(node->child)
    {
        translate_DecList(node->child->bro);
    }
}

void translate_DecList(treeNode *node)
 {
    if (node==NULL)
        return ;
    //DecList -> Dec
    if(node->child->bro == NULL) 
    {
       translate_Dec(node->child);
    }
    //DecList -> Dec COMMA DecList
    else   
    {
        translate_Dec(node->child);
        translate_DecList(node->child->bro->bro);
    }
}

void  translate_Dec(treeNode *node) 
{
    if (node==NULL)
        return ;
    //Dec ->VarDec
    if(node->child->bro == NULL)
    {
        translate_VarDec(node->child);
    }
    //Dec -> VarDec ASSIGNOP Exp
    else   
    {
        FieldList* paralist = (FieldList*)malloc(sizeof(FieldList));
        Type * L = translate_VarDec(node->child);
        Type * R = translate_Exp(node->child->bro->bro);
    }
}