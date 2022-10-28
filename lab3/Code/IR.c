#include"IR.h"

int varCount = 1, tempCount = 1, labelCount = 1;


Operand* newLabel()
{

}

Operand* newTemp()
{

}

Operand *newConstant(int val)
{
    
}

void createSingle(int kind,Operand*op)
{

}

void createCond(Operand* op1, Operand* op2, Operand* target, char* re)
{

}

void createAssign( Operand* left, Operand* right)
{

}

void createBinop(unsigned kind,  Operand* res, Operand* op1, Operand* op2)
{

}

void createSinop(unsigned kind, Operand* res, Operand* op)
{

}


void translate_init()
{

}

//为符号表中符号创建op,在semantic添加符号时调用。
Operand* new_symbol_op(char*name,Type* type)
{
    if(type==NULL)
        return NULL;
    Operand* op = (Operand*) malloc(sizeof(Operand));
    if(type->kind==BASIC)
    {
        op->kind = VARIABLE_O;
        op->u.var_no = varCount++;
        op->type = VAL_O;
        return op;
    }
    if(type->kind==FUNCTION)
    {
        op->kind = FUNCTION_O;
        op->u.func_name = name;
    }
}

void Translate(treeNode *root)
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
				translate_Exp(node->child,NULL);
			}
            //Stmt -> CompSt
			else if(!strcmp("CompSt",node->child->name )) 
			{
				translate_CompSt(node->child);
			}
            //Stmt -> RETURN Exp SEMI
			else if(!strcmp( "RETURN",node->child->name)) 
			{
                Operand* t1 = newTemp();
                translate_Exp(node->child->bro, t1);
                createSingle(RETURN_I, t1);
			}
            //Stmt -> IF LP Exp RP Stmt 
            else if(!strcmp("IF",node->child->name)&&node->child->bro->bro->bro->bro->bro==NULL)
            {
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                translate_Cond(node->child->bro->bro, label1, label2);
                createSingle(LABEL_I, label1);
                translate_Stmt(node->child->bro->bro->bro->bro);
                createSingle(LABEL_I, label2);
            }
            //Stmt -> IF LP Exp RP Stmt ELSE Stmt 
            else if(!strcmp("IF",node->child->name))
            {
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                Operand* label3 = newLabel();
                translate_Cond(node->child->bro->bro, label1, label2);
                createSingle(LABEL_I, label1);
                translate_Stmt(node->child->bro->bro->bro->bro);
                createSingle(GOTO_I, label3);
                createSingle(LABEL_I, label2);
                translate_Stmt(node->child->bro->bro->bro->bro->bro->bro);
                createSingle(LABEL_I, label3);
            }
            // WHILE LP Exp RP Stmt
			else if(!strcmp("WHILE",node->child->name)) 
			{
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                Operand* label3 = newLabel();
                createSingle(LABEL_I, label1);
                translate_Cond(node->child->bro->bro, label2, label3);
                createSingle(LABEL_I, label2);
                translate_Stmt(node->child->bro->bro->bro->bro);
                createSingle(GOTO_I, label1);
                createSingle(LABEL_I, label3);
			}
		}
}

void translate_Cond(treeNode* node, Operand* label_true, Operand* label_false)
{
    if(!node)
        return;
    // Exp AND Exp
    if(node->child->bro&&strcmp("AND",node->child->bro->name))
    {
        Operand* label1 = newLabel();
        translate_Cond(node->child, label1, label_false);
        createSingle(LABEL_I, label1);
        translate_Cond(node->child->bro->bro, label_true, label_false);
    } 
     // Exp OR Exp
     else if(node->child->bro&&strcmp("OR",node->child->bro->name))
    {
        Operand* label1 = newLabel();
        translate_Cond(node->child, label_true, label1);
        createSingle(LABEL_I, label1);
        translate_Cond(node->child->bro->bro, label_true, label_false);
    } 
    // Exp RELOP Exp
    else if(node->child->bro&&strcmp("RELOP",node->child->bro->name))
    {
        Operand* t1 = newTemp(0);
        Operand* t2 = newTemp(0);
        translate_Exp(node->child, t1);
        translate_Exp(node->child->bro->bro, t2);
        createCond(t1, t2, label_true, node->child->bro->s_val);
        createSingle(GOTO_I, label_false);
    } 
    // NOT Exp
    else if(node->child&&strcmp("NOT",node->child->s_val))
        translate_Cond(node->child->bro,label_false, label_true);
    else {
        Operand* t1 = newTemp();
        translate_Exp(node, t1);
        createCond(t1, newConstant(0), label_true, "!=");
        createSingle(GOTO_I, label_false);
    } 
}

void translate_Exp(treeNode* node,Operand* place)
{
    if (node==NULL)
            return ;
    //Exp->INT
     if(!strcmp("INT",node->child->name))
        {
            if(place!=NULL)
            {
                Operand* op = (Operand*)malloc(sizeof(Operand));
                op->kind = CONSTANT_O;
                op->u.value = node->child->i_val;
                createAssign(place, op);
            }
        }
    //Exp->FLOAT
    else if(!strcmp("FLOAT",node->child->name))
        {
            //不会出现
        }
    else if(!strcmp("Exp",node->child->name))
        {
            if(node->child->bro==NULL)
                return ;
            //Exp -> Exp ASSIGNOP Exp
            if(!strcmp("ASSIGNOP",node->child->bro->name))
            {
                treeNode* E1 = node->child;
                treeNode* E2 = node->child->bro->bro;
                //E1->ID 
                if(E1->child&&!strcmp("ID",E1->child->name)&&!E1->child->bro)
                {
                    Operand* t1 = newTemp();
                    translate_Exp(E1,t1);
                    Operand* t2 = newTemp();
                    translate_Exp(E2,t2);
                    createAssign(t1 , t2);
                    if (place) 
                        createAssign( place, t1);
                }
                else if(E1->child&&!strcmp("Exp",E1->child->name))
                {
                    // E1->Exp LB Exp RB 
                    if(E1->child->bro&&!strcmp("LB",E1->child->bro->name))
                    {

                    }
                    //E1->Exp DOT ID
                    else if(E1->child->bro&&!strcmp("DOT",E1->child->bro->name))
                    {
                        
                    }
                }
            }
            //Exp -> Exp LB Exp RB
            else if(!strcmp("LB",node->child->bro->name))
            {
                //translate_Exp(node->child->bro->bro);
            }
            //Exp -> Exp DOT ID
            else if(!strcmp("DOT",node->child->bro->name))
            {
                //translate_Exp(node->child);
            }
            //Exp -> Exp AND/OR/RELOP/PLUS/MINUS/STAR/DIV EXP
            else 
            {
                if(!strcmp("RELOP",node->child->bro->name)||!strcmp("AND",node->child->bro->name)||!strcmp("OR",node->child->bro->name))
                {
                    Operand* label1 = newLabel();
                    Operand* label2 = newLabel();
                    if (place)
                        {
                            createAssign(place, newConstant(0));
                        }
                    translate_Cond(node, label1, label2);
                    createSingle(LABEL_I, label1);
                    if (place)
                        {
                            createAssign(place,newConstant(1));
                        }
                    createSingle(LABEL_I, label2);
                }
                else if(!strcmp("PLUS",node->child->bro->name)||!strcmp("MINUS",node->child->bro->name)||!strcmp("STAR",node->child->bro->name)||!strcmp("DIV",node->child->bro->name))
                {
                        treeNode* E1 = node->child;
                        treeNode* E2 = node->child->bro->bro;
                        Operand* t1 = newTemp();
                        translate_Exp(E1,t1);
                        Operand* t2 = newTemp();
                        translate_Exp(E2,t2);
                        if (place)
                        {
                            if(!strcmp("PLUS",node->child->bro->name))
                                createBinop(ADD_I, place, t1, t2);
                            if(!strcmp("MINUS",node->child->bro->name))
                                createBinop(SUB_I, place, t1, t2);
                            if(!strcmp("STAR",node->child->bro->name))
                                createBinop(MUL_I, place, t1, t2);
                            if(!strcmp("DIV",node->child->bro->name))
                                createBinop(DIV_I, place, t1, t2);
                        }
                    }
            }
        }
    //EXP-> LP Exp RP | MINUS EXP
    else if(!strcmp("LP",node->child->name)||!strcmp("MINUS",node->child->name))
        {
            if(!strcmp("LP",node->child->name))
                translate_Exp(node->child->bro, place);
            else if(!strcmp("MINUS",node->child->name))
            {
                Operand* t1 = newTemp();
                translate_Exp(node->child->bro, t1);
                if (place)
                    createBinop(SUB_I, place, newConstant(0), t1);
            }
        } 
    //EXP-> NOT EXP
    else if(!strcmp("NOT",node->child->name))
        {
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                if (place)
                    {
                        createAssign(place, newConstant(0));
                    }
                translate_Cond(node, label1, label2);
                createSingle(LABEL_I, label1);
                if (place)
                    {
                        createAssign(place,newConstant(1));
                    }
                createSingle(LABEL_I, label2);
        } 
    //EXP-> ID | ID LP RP | ID LP Args RP
    else if(!strcmp("ID",node->child->name))
            {
                //EXP-> ID
                if(node->child->bro==NULL)
                {
                    if (place) 
                    {
                        hashNode* symbol = search(node->child->s_val);
                        createAssign(place, symbol->op);
                    }
                }
            }
    //ID LP Args RP
    else if(!strcmp("Args",node->child->bro->bro->name))
    {
        hashNode* func = search(node->child->s_val);
        int arg_count = func->type->u.function.argCount;
        Operand * arg_list[arg_count];
        translate_Args(node->child->bro->bro,arg_list,arg_count-1);
        if (!strcmp(func->name, "write")) 
        {
            createSingle(WRITE_I, arg_list[0]);
            if (place)
                createAssign(place, newConstant(0));
        } 
        else {
            for (int i = 0; i < arg_count; ++i) {
                createSingle(ARG_I, arg_list[i]);
            }
            Operand* op = (Operand*)malloc(sizeof(Operand));
            op->kind = FUNCTION_O;
            op->u.func_name = func->name;
            if (place)
                createSinop(CALL_I, place, op);
            else {
                Operand* t1 = newTemp();
                createSinop(CALL_I, t1, op);
            }
}
    }
    //EXP-> ID LP RP |
    else
            {
                hashNode* func = search(node->child->s_val);
                if(!strcmp(func->name,"READ_I"))
                {
                    if (place)
                            createSingle(READ_I, place);
                    else {
                        Operand* t1 = newTemp();
                        createSingle(READ_I, t1);
                    }
                }
                else
                {
                    Operand* op = (Operand*)malloc(sizeof(Operand));
                    op->kind = FUNCTION_O;
                    op->u.func_name = func->name;
                    if (place)
                        createSinop(CALL_I, place, op);
                    else {
                        Operand* t1 = newTemp();
                        createSinop(CALL_I, t1, op);
                    }
                }
            }
}

void  translate_Args(treeNode* node,Operand* arg_list[],int head)
{
    if(node==NULL)
        return;
        // :Exp COMMA Args
    if(node->child->bro!=NULL)
    {
        Operand* t1= newTemp();
        translate_Exp(node->child,t1);
        arg_list[head]=t1;
        translate_Args(node->child->bro->bro,arg_list,head--);
    }
    //:Exp
    else
    {
        Operand* t1= newTemp();
        translate_Exp(node->child,t1);
        arg_list[head]=t1;
    }
}

void translate_FunDec(treeNode* node)
{
    if (node==NULL)
             return;
    Operand* func = (Operand*)malloc(sizeof(Operand));
    func->kind = FUNCTION_O;
    func->u.func_name = node->child->s_val;
    createSingle(FUNCTION_I, func);
    //FunDec -> ID LP VarList RP
    if(!strcmp( "VarList",node->child->bro->bro->name))   
    {
        translate_VarList(node->child->bro->bro,1);
    }
    //FunDec -> ID LP RP
    else    
    {

    }

}

//0为默认，1为函数参数，2为结构体域
void  translate_VarList(treeNode* node,int headType)
{
        if (node==NULL)
             return;
        // VarList -> ParamDec COMMA VarList
        if(node->child->bro)  
        {
             translate_ParamDec(node->child,headType);
            translate_VarList(node->child->bro->bro,headType);
        }
        //VarList -> ParamDec
        else    
        {
            translate_ParamDec(node->child,headType);
        }
}

void translate_ParamDec(treeNode* node,int headType)
{
        if (node==NULL)
             return;
        FieldList* temp = (FieldList*)malloc(sizeof(FieldList));
        //Specifier VarDec
        translate_VarDec(node->child->bro,headType);
}

void translate_ExtDecList(treeNode* node)
{
    if (node==NULL)
        return;
    //ExtDecList -> VarDec COMMA ExtDecList
    if(node->child->bro!=NULL) 
    {
        translate_VarDec(node->child,0); 
        translate_ExtDecList(node->child->bro->bro);
    }
    //ExtDecList -> VarDec
    else 
    { 
        translate_VarDec(node->child,0);
    }
}

//name[INT][INT]，返回name的类型
void translate_VarDec(treeNode* node,int headType)
{
    if (node==NULL)
        return ;
        //VarDec -> ID
        if(!strcmp("ID",node->child->name))
        {
            //函数参数
            if (headType==1) 
            {
                hashNode* sym = search(node->child->s_val);
                sym->op->kind = PARAMETER_O;
                createSingle(PARAM_I, sym->op);
            }
            //结构体
            else if(headType==2)
            {

            }
        }
        //VarDec -> VarDec LP INT RP
        else  
        {	
            translate_VarDec(node->child,headType);
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
        translate_VarDec(node->child,0);
    }
    //Dec -> VarDec ASSIGNOP Exp
    else   
    {
        translate_VarDec(node->child,0);
        hashNode* res = search(node->child->child->s_val);
        Operand* t1 = newTemp();
        translate_Exp(node->child->bro->bro, t1);
        createAssign(res->op, t1);
    }
}