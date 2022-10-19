#include"semantic.h"

hashNode* hashTable[HASHSIZE+1];

void tableInit()
{
    for(int i=0;i<=HASHSIZE;++i)
        hashTable[i]=NULL;
}

unsigned hash_pjw( const char* name)
{
    unsigned val = 0,i;
    for(;*name;++name)
    {
        val = (val<<2) + *name;
        if (i = val & ~HASHSIZE)    val = (val^(i>>12))&HASHSIZE;
    }
    return val;
}

void printTable()
{
for (int i = 0; i < HASHSIZE + 1; ++i) {
        if (!hashTable[i])
            continue;
        else
        printf("name:%s line:%d is defined:%d size:%d Type:%d\n ",hashTable[i]->name,hashTable[i]->line,hashTable[i]->defined,hashTable[i]->size,hashTable[i]->type->kind);
}
}

Type* getTypeFromTable(const char * name)
{
    hashNode* res=search(name);
    if(res)
        return res->type;
    return NULL;
}

Type* getTypeFromStruct(const char * name,Type*structure)
{
    if(!structure)
        return NULL;
    FieldList *cur = structure->u.structure;
    while(cur)
    {
        if(!strcmp(name,cur->name))
            return cur->type;
        cur=cur->tail;
    }
    return NULL;
}

void insert(hashNode* node)
{
    unsigned index = hash_pjw(node->name);
    node->next=hashTable[index];
    hashTable[index]=node;
}

hashNode* search(const char * name)
{
    unsigned index =  hash_pjw(name);
    hashNode* head = hashTable[index];
    while(head!=NULL)
    {
        if(!strcmp(head->name,name))
            break;
        head=head->next;
    }
    return head;
}

hashNode* newSymbol(const char* name, Type* type, int line, int defined)
{
    hashNode* temp = (hashNode*)malloc(sizeof(hashNode));
    temp->name=name;
    temp->type = type;
    temp->line = line;
    temp->defined = defined;
        
    if(temp->type->kind==BASIC)
    {
        temp->size=32;
    }
    else if(temp->type->kind==ARRAY)
    {
        Type * stack [100],*cur=type;
        int top=0;
        while(cur->kind!=BASIC)
        {
            stack[top]=cur;
            cur=cur->u.array.elem;
            ++top;
        }
        int size=32;
        while(top>0)
        {
            cur=stack[--top];
            size*=cur->u.array.size;
        }
        temp->size=size;
    }
    else if(temp->type->kind==STRUCTURE)
    {
    }
    else if(temp->type->kind==FUNCTION)
    {

    }
    return temp;
}

void addSymbol(const char* name, Type* type, int line, int defined)
{
    if(!type)
        return;    
    int index =hash_pjw(name);
    //第一次出现该符号
    if(hashTable[index]==NULL)
        insert(newSymbol(name, type, line, defined));
    else
    {
        //找出与该符号名字相同的节点，判断声明、定义等等情况
        hashNode*cur=hashTable[index];
        while(cur)
        {
            if(!strcmp(cur->name,name))
                break;
            cur = cur->next;
        }
        //第一次出现该符号
        if(cur==NULL)
             insert(newSymbol(name, type, line, defined));
        else
        {
            //重复定义
            if(cur->defined==1&&defined==1)
            {
                
                if(type->kind==BASIC||type->kind==ARRAY||(type->kind==STRUCTURE&&type->is_var==1))
                     serror(Type3,line,"Redefined variable");
                else if(type->kind==FUNCTION)
                    serror(Type4,line,"Redefined function");
                else if(type->kind==STRUCTURE&&type->is_var==0)
                    serror(Type16,line,"Redefined structure");
            }
            //检查新的声明是否符合
            else if(cur->defined==1&&defined==0)
            {
                 if(!checkDeclaration(cur->type,type))
                    serror(Type19,line,"conflict of declaration");
            }
            //检查新的定义与旧的声明是否相符合
            else if(cur->defined==0&&defined==1)
            {
                 if(!checkDeclaration(cur->type,type))
                    serror(Type19,line,"conflict of declaration");
                    cur->defined=1;
            }
            //检查老旧声明是否相符合
            else if(cur->defined==0&&defined==0)
            {
                    if(!checkDeclaration(cur->type,type))
                        serror(Type19,line,"conflict of declaration");
            }

        }
    }
}

int isDoInStructure(const char* domain,Type*structure)
{
    if(!structure)
        return 0;
    FieldList *cur = structure->u.structure;
    while(cur)
    {
        if(!strcmp(domain,cur->name))
            return 1;
        cur=cur->tail;
    }
    return 0;
}

//检查是否所有声明的函数是否定义
void checkDeclarationAndDefine()
{
    for(int i =0;i<HASHSIZE;++i)
    {
        hashNode* Node = hashTable[i];
        if(!Node)
            continue;
        if(Node->type->kind==FUNCTION&&Node->defined==0)
        {
            serror(Type18,Node->line,"Undefined Function");
        }
    }
}

void Program(treeNode* node)
{
    if (node==NULL)
        return;
    
    if (node->child!=NULL) 
    {
        ExtDefList(node->child);
    }
    checkDeclarationAndDefine();
}

void ExtDefList(treeNode* node)
{
        if (node==NULL)
             return;
        ExtDef(node->child);
        ExtDefList(node->child->bro);
}

void ExtDef(treeNode* node)
{
    if (node==NULL)
             return;
     // Specifier ExtDecList SEMI
     if(!strcmp("ExtDecList",node->child->bro->name))
     {
        Type* type = Specifier(node->child);
        ExtDecList(node->child->bro,type);
    }
     else if(!strcmp("FunDec",node->child->bro->name))
     {
        Type* type = Specifier(node->child);
        // Specifier FunDec CompSt
        if(!strcmp("CompSt",node->child->bro->bro->name))
            {
                FunDec(node->child->bro,type,1);
                CompSt(node->child->bro->bro,type,1,1);
            }
        //Specifier FunDec SEMI
        else
            FunDec(node->child->bro,type,0);
     }
    // Specifier SEMI
    else
    {
        Specifier(node->child);
    }
}

void CompSt(treeNode* node, Type* ret, int defined,int depth) 
{	 
    if (node==NULL)
             return ;
        //CompSt -> LC DefList StmtList RC
		if(!strcmp("DefList",node->child->bro->name)) 
		{
			DefList(node->child->bro,NULL, defined,depth);
			if(!strcmp("StmtList",node->child->bro->bro->name))
				StmtList(node->child->bro->bro, ret,depth);
		}
        //CompSt -> LC StmtList RC
		else if(!strcmp("StmtList",node->child->bro->name))
				StmtList(node->child->bro, ret,depth);
		
}

void StmtList(treeNode* node, Type* ret,int depth)
{
    if (node==NULL)
             return ;
		if(node->child) 
		{
            //StmtList -> Stmt StmtList
			if(!strcmp("Stmt",node->child->name)) 
			{
				Stmt(node->child, ret,depth);
				StmtList(node->child->bro, ret,depth);
			}
			else{
			//StmtList -> empty
			}
		}
}

void Stmt(treeNode* node, Type* ret,int depth)
{
    if (node==NULL)
             return ;
		if(node->child)
		{ 
            //Stmt -> Exp SEMI
			if(!strcmp("Exp",node->child->name ))
			{

				Exp(node->child);
			}
            //Stmt -> CompSt
			else if(!strcmp("CompSt",node->child->name )) 
			{
				CompSt(node->child, ret,1,depth+1);
			}
            //Stmt -> RETURN Exp SEMI
			else if(!strcmp( "RETURN",node->child->name)) 
			{
				Type* type = Exp(node->child->bro);
				if(!BeSameType(type, ret))
				{
					serror(Type8,node->child->bro->line,"Type mismatched for return");
				}
			}
            //Stmt -> IF LP Exp RP Stmt | IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt
			else if(!strcmp("IF",node->child->name) || !strcmp("WHILE",node->child->name )) 
			{
				Exp(node->child->bro->bro);
				Stmt(node->child->bro->bro->bro->bro, ret,depth);
                //Stmt ->IF LP Exp RP Stmt ELSE Stmt 
				if(node->child->bro->bro->bro->bro->bro!=NULL)  
				{
					Stmt(node->child->bro->bro->bro->bro->bro->bro, ret,depth);
				}
			}
		}
}

Type* Exp(treeNode* node)
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
            Type* type1 = Exp(node->child);
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
                Type *type2 = Exp(node->child->bro->bro);
                if(!BeSameType(type1,type2))
                    serror(Type5,E1->line,"The mismatched for assignment");
                return type1;
            }
            //Exp -> Exp LB Exp RB
            else if(!strcmp("LB",node->child->bro->name))
            {
                Type* para = Exp(node->child->bro->bro);
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
                Type* type2 = Exp(node->child->bro->bro);
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
                type=Exp(node->child->bro);
                return type;
            } 
        //EXP-> NOT EXP
        else if(!strcmp("NOT",node->child->name))
            {
                type=Exp(node->child->bro);
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
                        Args(node->child->bro->bro,type);
                    else if(argflag==1)
                         serror(Type9,node->child->line,"function need arguments");
                    return res;
                }
            }
}

void Args(treeNode* node, Type* type)
{
    if(node==NULL||type==NULL)
        return;
    if(type->kind!=FUNCTION)
        return;
    //出现错误置1
    int errorflag = 0;
    treeNode*cur = node;
    FieldList *paralist=type->u.function.para;
    while(cur->child)
    {
        if(paralist==NULL)
            {
                errorflag=1;
                break;
            }
        Type* temp = Exp(cur->child);
        if(!BeSameType(temp,paralist->type))
            {
                errorflag=1;
                break;
            }
        if(cur->child->bro)
        {
            cur = cur->child->bro->bro;
            paralist=paralist->tail;
        }
        else
            break;
    }
    //paralist过长，或以上报错
    if(errorflag==1||(paralist!=NULL&&paralist->tail!=NULL))
    {
        serror(Type9,cur->child->line,"function has wrong arguments");
    }
}

Type* Specifier(treeNode* node)
{
    if (node==NULL)
             return NULL;
    //Specifier -> TYPE
    if(!strcmp( "TYPE",node->child->name))  
    {
        Type* type = (Type*)malloc(sizeof(Type));
        type->kind = BASIC;
        if(!strcmp("int",node->child->s_val))
            type->u.basic = 0;
        else
            type->u.basic = 1;
        return type;
    }
    //Specifier -> StructSpecifier
    else    
        return StructSpecifier(node->child);
}

Type* StructSpecifier(treeNode* node)
{
    if (node==NULL)
            return NULL;
    Type* type = NULL;
    //StructSpecifier -> STRUCT OptTag LC DefList RC
    if(!strcmp("OptTag",node->child->bro->name ))
    {
        type = (Type*)malloc(sizeof(Type));
        type->kind = STRUCTURE;
        type->u.structure = NULL;
            char* name;
            //OptTag -> ID
            if(node->child->bro->child)    
                {
                    name = node->child->bro->child->s_val;
                    DefList(node->child->bro->bro->bro, type,0,-1);
                    addSymbol(name, type, node->line, 1);
                }
            else
                {
                    DefList(node->child->bro->bro->bro, type,0,-1);
                }
            return type;
    }
    //StructSpecifier -> STRUCT LC DefList RC
    else if(!strcmp("LC",node->child->bro->name ))
    {
        type = (Type*)malloc(sizeof(Type));
        type->kind = STRUCTURE;
        type->u.structure = NULL;
        DefList(node->child->bro->bro, type,0,-1);
        return type;
    }
    //StructSpecifier ->STRUCT Tag
    else
    {
            type= getTypeFromTable(node->child->bro->child->s_val);
            if(type == NULL)
                serror(Type17,node->child->bro->line,"Undefined structure");
            return type;
    }
    
}

void FunDec(treeNode* node, Type* ret, int defined)
{
    if (node==NULL)
             return;
        Type* type = (Type*)malloc(sizeof(Type));
        type->kind = FUNCTION;
        type->u.function.ret = ret;
        //FunDec -> ID LP VarList RP
        if(!strcmp( "VarList",node->child->bro->bro->name))   
        {
            VarList(node->child->bro->bro, type);
        }
        //FunDec -> ID LP RP
        else    
        {
            type->u.function.para = NULL;
        }

        //定义函数，将所有参数也加入符号表
        if(defined)
        {
            FieldList* para = type->u.function.para;
            while(para)
            {
                addSymbol(para->name, para->type, node->child->line, 1);
                para = para->tail;
            }
        }
        addSymbol(node->child->s_val, type, node->child->line, defined);
}


void  VarList(treeNode* node, Type* headType)
{
        if (node==NULL)
             return;
        // VarList -> ParamDec COMMA VarList
        if(node->child->bro)  
        {
            ParamDec(node->child, headType);
            VarList(node->child->bro->bro, headType);
        }
        //VarList -> ParamDec
        else    
        {
            ParamDec(node->child, headType);
        }
}

void ParamDec(treeNode* node, Type* headType)
{
        if (node==NULL)
             return;
        FieldList* temp = (FieldList*)malloc(sizeof(FieldList));
        //Specifier VarDec
        Type* type = Specifier(node->child);
        VarDec(node->child->bro, type, headType,0,temp);
}

void ExtDecList(treeNode* node, Type* type)
{
    if (node==NULL)
        return;
    //ExtDecList -> VarDec COMMA ExtDecList
    if(node->child->bro!=NULL) 
    {
        VarDec(node->child, type,NULL, 1,NULL); 
        ExtDecList(node->child->bro->bro,type);
    }
    //ExtDecList -> VarDec
    else 
    { 
        VarDec(node->child, type, NULL,1,NULL);
    }
}

//name[INT][INT]，返回name的类型
Type* VarDec(treeNode* node, Type *type, Type* headType,int defined,FieldList* paralist)
{
    if (node==NULL)
        return NULL;
    if(type==NULL)
        return NULL;

    if(headType==NULL||headType->kind==FUNCTION||headType->kind==STRUCTURE)
    {
        //VarDec -> ID
        if(!strcmp("ID",node->child->name))
        {
            Type* temp = (Type*)malloc(sizeof(Type));   
            temp->kind=type->kind;
            temp->u=type->u;
            temp->is_var=1;
            if(paralist)
            {
                paralist->name=node->child->s_val;
                paralist->type=temp;
                paralist->tail=NULL;
                //检查结构体域内变量
                if(headType&&headType->kind==STRUCTURE)
                {
                    if(headType->u.structure==NULL)
                        headType->u.structure=paralist;
                    else
                    {
                        FieldList *head=headType->u.structure,*tail=head;
                        while(head)
                        {
                            if(!strcmp(head->name,paralist->name))
                                serror(Type15,node->child->line,"Redefined var in structure");
                            head=head->tail;
                            if(tail->tail)
                                tail=tail->tail;
                        }
                        
                        tail->tail=paralist;
                    }
                }
                else if(headType&&headType->kind==FUNCTION)
                {
                    if(headType->u.function.para==NULL)
                        headType->u.function.para=paralist;
                    else
                    {
                        FieldList *tail = headType->u.function.para;
                        while(tail&&tail->tail)
                            tail=tail->tail; 
                        tail->tail=paralist;
                    }
                }
            }
            if(!headType)
                addSymbol(node->child->s_val, temp, node->child->line, defined);
            return temp;
        }
        //VarDec -> VarDec LP INT RP
        else  
        {	
            Type* temp = (Type*)malloc(sizeof(Type));
            temp->kind = ARRAY;
            temp->u.array.elem = type;   
            temp->u.array.size = node->child->bro->bro->i_val;
            return VarDec(node->child, temp, headType,defined,paralist);
        }
    }
    else if(headType->kind==STRUCTURE)
    {

    }
}

void DefList(treeNode* node, Type* headType,int defined,int depth)
{
    if (node==NULL)
        return ;
     // DefList-> Def DefList    
    if(node->child)
    {
        Def(node->child, headType,defined);
        DefList(node->child->bro, headType,defined,depth);              
    }
    // DefList-> empty
    else
    {
        return;
    }
}

void Def(treeNode* node, Type * headType,int defined)
{
    if (node==NULL)
        return;
    //Def -> Specifier DecList SEMI
    if(node->child)
    {
        Type* type = Specifier(node->child);
        if(type == NULL) 
            return ;
        DecList(node->child->bro, type, headType,defined);
    }
}

void DecList(treeNode *node, Type* type, Type*headType, int defined)
 {
    if (node==NULL)
        return ;
    //DecList -> Dec
    if(node->child->bro == NULL) 
    {
       Dec(node->child, type, headType,defined);
    }
    //DecList -> Dec COMMA DecList
    else   
    {
        Dec(node->child, type, headType,defined);
        DecList(node->child->bro->bro, type,headType, defined);
    }
}

void  Dec(treeNode *node, Type* type,  Type* headType,int defined) 
{
    if (node==NULL)
        return ;
    //Dec ->VarDec
    if(node->child->bro == NULL)
    {
        FieldList* paralist = (FieldList*)malloc(sizeof(FieldList));
        VarDec(node->child, type, headType,defined,paralist);
    }
    //Dec -> VarDec ASSIGNOP Exp
    else   
    {
        FieldList* paralist = (FieldList*)malloc(sizeof(FieldList));
        Type * L = VarDec(node->child, type, headType,defined,paralist);
        Type * R = Exp(node->child->bro->bro);
        if (headType&&headType->kind==STRUCTURE)
        {
            serror(Type15,node->child->line,"Initial in structure");
        }
        
        if(!checkOperator(L,R,"ASSIGNOP"))
        {
            serror(Type5,node->child->line,"Type mismatched for assignment");
        }
    }
}

//检查之是否是相同类型
int BeSameType(Type* Ltype,Type*Rtype)
{    
    if(Ltype==NULL||Rtype==NULL)
        return 0;
    //函数类型先把其转化为返回值类型
    if(Ltype->kind==FUNCTION||Rtype->kind==FUNCTION)
    {
        Type *t1= Ltype->kind==FUNCTION?Ltype->u.function.ret:Ltype;
        Type *t2= Rtype->kind==FUNCTION?Rtype->u.function.ret:Rtype;
        if (!BeSameType(t1,t2))
        {
            return 0;
        }
        return 1;
    }
    if(Ltype->kind!=Rtype->kind)
        return 0;
    if(Ltype->kind==BASIC)
    {
        if(Ltype->u.basic!=Rtype->u.basic)
            return 0;
        return 1;
    }
    if(Ltype->kind==ARRAY)
    {
        if(Ltype->u.array.size != Rtype->u.array.size||!BeSameType(Ltype->u.array.elem,Ltype->u.array.elem))
            return 0;
        return 1;
    }
    if(Ltype->kind==STRUCTURE)
    {
        FieldList *p1=Ltype->u.structure,*p2=Rtype->u.structure;
        while(p1&&p2)
        {
            if(!BeSameType(p1->type,p2->type))
                return 0;
            p1=p1->tail;
            p2=p2->tail;
        }
        if(p1||p2)
            return 0;
        return 1;
    }
}

//检查两个函数类型是否一致
int checkDeclaration(Type* Ltype,Type*Rtype)
{
    if(!Ltype||!Rtype)
        return 0;
    //检查返回值
    if(!BeSameType(Ltype,Rtype))
        return 0;
    //检查参数
    FieldList*p1=Ltype->u.function.para,*p2=Rtype->u.function.para;
    while(p1&&p1)
    {
        if(!BeSameType(p1->type,p2->type))
            return 0;
            p1=p1->tail;
            p2=p2->tail;
    }
    if(p1||p2)
        return 0;
    return 1;
    }

int checkOperator(Type* Ltype,Type*Rtype,const char* operator)
{
    if(!strcmp("ASSIGNOP",operator))
    {
        if(!BeSameType(Ltype,Rtype))
            return 0;
    }
    else if(!strcmp("AND",operator)||!strcmp("OR",operator))
    {

        if(!BeSameType(Ltype,Rtype))
            return 0;
        //只能为整数
        if(Ltype->kind!=BASIC||Rtype->kind!=BASIC)
            return 0;
        if(Ltype->u.basic!=0||Rtype->u.basic!=0)
            return 0;
    }
    else if(!strcmp("RELOP",operator))
    {
        if(!BeSameType(Ltype,Rtype))
            return 0;
        if(Ltype->kind!=BASIC||Rtype->kind!=BASIC)
            return 0;
    }
    else if(!strcmp("PLUS",operator)||!strcmp("MINUS",operator)||!strcmp("STAR",operator)||!strcmp("DIV",operator))
    {
        if(!BeSameType(Ltype,Rtype))
            return 0;
    }

    return 1;
}

void serror(int errorType,int line,char* msg)
{
    printf("Error type %d at Line %d: %s.\n",errorType,line,msg);
}

