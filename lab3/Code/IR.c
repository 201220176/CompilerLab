#include"IR.h"

int varCount = 1, tempCount = 1, labelCount = 1;
InterCodes* head;
InterCodes* tail;


char* toOp(Operand* op)
{
    char* str = (char*)malloc(100);
    if(!op)
    {
        str[0]='\0';
        return str;
    }
    switch (op->kind) 
    {
    case VARIABLE_O:
        sprintf(str, "v%d", op->u.var_no);
        break;
    case TEMP_O:
        sprintf(str, "t%d", op->u.var_no);
        break;    
    case LABEL_O:
        sprintf(str, "label%d", op->u.var_no);
        break;    
    case CONSTANT_O:
        sprintf(str, "#%d", op->u.value);
        break;    
    case FUNCTION_O:
        sprintf(str, "%s", op->u.func_name);
        break;
    case PARAMETER_O:
        sprintf(str, "v%d", op->u.var_no);
        break;
    default:
        break;
    }
    return str;
}

void writeToFile(char* fielname)
{
    FILE* f =fopen(fielname,"w");
    if(!head)
        return;
    InterCodes* cur = head->next;
    while(cur)
    {
        switch (cur->code.kind) 
        {
            case LABEL_I:
                fprintf(f, "LABEL %s :\n", toOp(cur->code.u.single.op));
                break;
            case FUNCTION_I:
                fprintf(f, "FUNCTION %s :\n", toOp(cur->code.u.single.op));
                break;
            case ARG_I:
                fprintf(f, "ARG %s\n", toOp(cur->code.u.single.op));
                break;
            case DEC_I:
                fprintf(f, "DEC %s %d\n", toOp(cur->code.u.dec.op), cur->code.u.dec.size);
                break;
            case ASSIGN_I: {
                    switch (cur->code.u.assign.type) {
                    case NORMAL_I:
                        fprintf(f, "%s := %s\n", toOp(cur->code.u.assign.left), toOp(cur->code.u.assign.right));
                        break;
                    case GETVAL_I:
                        fprintf(f, "%s := *%s\n", toOp(cur->code.u.assign.left), toOp(cur->code.u.assign.right));
                        break;
                    case SETVAL_I:
                        fprintf(f, "*%s := %s\n", toOp(cur->code.u.assign.left), toOp(cur->code.u.assign.right));
                        break;
                    case GETADDR_I:
                        fprintf(f, "%s := &%s\n", toOp(cur->code.u.assign.left), toOp(cur->code.u.assign.right));
                        break;
                    case COPY_I:
                         fprintf(f, "*%s := *%s\n", toOp(cur->code.u.assign.left), toOp(cur->code.u.assign.right));
                    break;
                    default:
                        break;
                    }
                } break;
            case ADD_I: {
                fprintf(f, "%s := %s + %s\n", toOp(cur->code.u.binop.res), toOp(cur->code.u.binop.op1),toOp(cur->code.u.binop.op2));
            } break;
            case SUB_I: {
                fprintf(f, "%s := %s - %s\n",toOp(cur->code.u.binop.res),toOp(cur->code.u.binop.op1), toOp(cur->code.u.binop.op2));
            } break;
            case MUL_I: {
                fprintf(f, "%s := %s * %s\n", toOp(cur->code.u.binop.res), toOp(cur->code.u.binop.op1), toOp(cur->code.u.binop.op2));
            } break;
            case DIV_I: {
                fprintf(f, "%s := %s / %s\n", toOp(cur->code.u.binop.res), toOp(cur->code.u.binop.op1), toOp(cur->code.u.binop.op2));
            } break;
            case IF_I: {
                fprintf(f, "IF %s %s %s GOTO %s\n",toOp(cur->code.u.cond.op1), cur->code.u.cond.relop, toOp(cur->code.u.cond.op2), toOp(cur->code.u.cond.target));
            } break;
            case GOTO_I:
                fprintf(f, "GOTO %s\n", toOp(cur->code.u.single.op));
                break;
            case RETURN_I:
                fprintf(f, "RETURN %s\n", toOp(cur->code.u.single.op));
                break;
            case CALL_I: {
                fprintf(f, "%s := CALL %s\n", toOp(cur->code.u.unaryop.res), toOp(cur->code.u.unaryop.op));
            } break;
            case PARAM_I:
                fprintf(f, "PARAM %s\n", toOp(cur->code.u.single.op));
                break;
            case READ_I:
                fprintf(f, "READ %s\n", toOp(cur->code.u.single.op));
                break;
            case WRITE_I:
                    fprintf(f, "WRITE %s\n", toOp(cur->code.u.single.op));
                break;
            default:
                break;
            }
        cur = cur->next;
    }
    fclose(f);
}

Operand* newLabel()
{
    Operand* op = (Operand*)malloc(sizeof(Operand));
    op->kind = LABEL_O;
    op->u.var_no = labelCount++;
    return op;
}

Operand* newTemp(int type)
{
    Operand* op = (Operand*)malloc(sizeof(Operand));
    op->kind = TEMP_O;
    if(type==ADDRESS_O)
        op->type=ADDRESS_O;
    else
        op->type=VAL_O;
    op->u.var_no = tempCount++;
    return op;
}

Operand *newConstant(int val)
{
    Operand* op = (Operand*)malloc(sizeof(Operand));
    op->kind = CONSTANT_O;
    op->u.value = val;
    return op;
}

void singleCode(int kind,Operand*op)
{
    if(!op)
        return;
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = kind;
    p->code.u.single.op = op;
    p->prev = tail;    
    p->next = NULL;
    tail->next = p;
    tail =p;
}

void condCode(Operand* op1, Operand* op2, Operand* target, char* re)
{
    if(!op1||!op2||!target)
        return;
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = IF_I;
    p->code.u.cond.op1 = op1;
    p->code.u.cond.op2 = op2;
    p->code.u.cond.target = target;
    p->code.u.cond.relop=(char*) malloc(4*sizeof(char));
    strcpy(p->code.u.cond.relop, re);
    p->prev = tail;    
    p->next = NULL;
    tail->next = p;
    tail =p;
}

void assignCode( Operand* left, Operand* right,int type)
{
    if(!left||!right)
        return;
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = ASSIGN_I;
    p->code.u.assign.type = type;
    p->code.u.assign.left = left;
    p->code.u.assign.right = right;
    p->prev = tail;    
    p->next = NULL;
    tail->next = p;
    tail =p;
}

void binCode(unsigned kind,  Operand* res, Operand* op1, Operand* op2)
{
    if(!res||!op1||!op2)
        return;
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = kind;
    p->code.u.binop.res = res;
    p->code.u.binop.op1 = op1;
    p->code.u.binop.op2 = op2;
    p->prev = tail;    
    p->next = NULL;
    tail->next = p;
    tail =p;
}

void unaryCode(unsigned kind, Operand* res, Operand* op)
{
    if(!res||!op)
        return ;
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = kind;
    p->code.u.unaryop.res = res;
    p->code.u.unaryop.op = op;
    p->prev = tail;    
    p->next = NULL;
    tail->next = p;
    tail =p;
}

void decCode(Operand* op, unsigned size)
{
    if(!op)
        return;
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = DEC_I;
    p->code.u.dec.op = op;
    p->code.u.dec.size = size;
    p->prev = tail;    
    p->next = NULL;
    tail->next = p;
    tail =p;
}

void translate_init()
{
    head = (InterCodes*)malloc(sizeof(InterCodes));
    head->prev = head->next = NULL;
    tail = head;
}

void arrayAssign(Type* E1type,Type* E2type,Operand*t1,Operand*t2)
{
    int size1 = E1type->u.array.size;
    int size2 = E2type->u.array.size;
    
    int s = size1>size2?size2:size1;
    for(int i=0;i<s;++i)
    {
        assignCode(t1,t2,COPY_I);
        binCode(ADD_I, t2, t2, newConstant(4));
        binCode(ADD_I, t1, t1, newConstant(4));
    }
}

//???????????????????????????op,???semantic????????????????????????
Operand* new_symbol_op(char*name,Type* type,int isfunctionpara)
{
    if(type==NULL)
        return NULL;
    Operand* op = (Operand*) malloc(sizeof(Operand));
    op->isfunctionpara=isfunctionpara;
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
    if(type->kind==STRUCTURE&&type->is_var==1)
    {
        op->kind = VARIABLE_O;
        op->u.var_no = varCount++;
        op->type = ADDRESS_O;
        return op;
    }
    if(type->kind==ARRAY)
    {
        op->kind = VARIABLE_O;
        op->u.var_no = varCount++;
        op->type = ADDRESS_O;
        return op;
    }
    return NULL;
}

int getOffsetInStruct(Type* type,char* domain,Type** returnType)
{
    if(!type)
    {
        *returnType=NULL;
        return 0;
    }
    FieldList*head =type->u.structure;
    int res = 0;
    while(head)
    {
        if(!strcmp(domain,head->name))
            break;
        res+=getTypeSize(head->type);
        head=head->tail;
    }
    *returnType=head->type;
    if(DEBUG)
        printf("getType in struct :%d\n",(*returnType)->kind);
    return res;
}

void Translate(treeNode *root)
{
    if(root == NULL)
        return;
    translate_init();
    translate_Program(root);
    if(DEBUG)
        exit(1);

}

void translate_Program(treeNode *node)
{
    if (node==NULL)
        return;
    if (node->child!=NULL) 
    {
        translate_ExtDefList(node->child);
        if(DEBUG)
            exit(2);
    }
}

void translate_ExtDefList(treeNode* node)
{
        if (node==NULL)
             return;
        translate_ExtDef(node->child);
        if(DEBUG)
            exit(3);
        translate_ExtDefList(node->child->bro);
}

void translate_ExtDef(treeNode* node)
{
    if (node==NULL)
             return;
     // Specifier ExtDecList SEMI
     if(!strcmp("ExtDecList",node->child->bro->name))
     {
        if(DEBUG)
            exit(4);
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
        {
            if(DEBUG)
            exit(6);
            translate_FunDec(node->child->bro);
        }
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

int stmtCount =0 ;

void translate_StmtList(treeNode* node)
{
    if (node==NULL)
             return ;
		if(node->child) 
		{
            //StmtList -> Stmt StmtList
			if(!strcmp("Stmt",node->child->name)) 
			{
                if(DEBUG)
                    printf("?????????????????????%d\n",node->line);
				translate_Stmt(node->child);
                ++stmtCount;
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
			if(!strcmp("Exp",node->child->name))
			{
				translate_Exp(node->child,NULL,0);
			}
            //Stmt -> CompSt
			else if(!strcmp("CompSt",node->child->name )) 
			{

				translate_CompSt(node->child);
			}
            //Stmt -> RETURN Exp SEMI
			else if(!strcmp( "RETURN",node->child->name)) 
			{
                
                Operand* t1 = newTemp(VAL_O);
                translate_Exp(node->child->bro, t1,0);
                singleCode(RETURN_I, t1);
			}
            //Stmt -> IF LP Exp RP Stmt 
            else if(!strcmp("IF",node->child->name)&&node->child->bro->bro->bro->bro->bro==NULL)
            {
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                translate_Cond(node->child->bro->bro, label1, label2);
                singleCode(LABEL_I, label1);
                translate_Stmt(node->child->bro->bro->bro->bro);
                singleCode(LABEL_I, label2);             
            }
            //Stmt -> IF LP Exp RP Stmt ELSE Stmt 
            else if(!strcmp("IF",node->child->name))
            {
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                Operand* label3 = newLabel();
                translate_Cond(node->child->bro->bro, label1, label2);
                singleCode(LABEL_I, label1);
                translate_Stmt(node->child->bro->bro->bro->bro);
                singleCode(GOTO_I, label3);
                singleCode(LABEL_I, label2);
                translate_Stmt(node->child->bro->bro->bro->bro->bro->bro);
                singleCode(LABEL_I, label3);
                
            }
            // WHILE LP Exp RP Stmt
			else if(!strcmp("WHILE",node->child->name)) 
			{
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                Operand* label3 = newLabel();
                singleCode(LABEL_I, label1);                                                               
                translate_Cond(node->child->bro->bro, label2, label3);
                singleCode(LABEL_I, label2);

                translate_Stmt(node->child->bro->bro->bro->bro);
                singleCode(GOTO_I, label1);
                singleCode(LABEL_I, label3);
			}
		}
}

int condCount=0;

void translate_Cond(treeNode* node, Operand* label_true, Operand* label_false)
{
    if(!node)
        return;
    // Exp AND Exp
    if(node->child->bro&&!strcmp("AND",node->child->bro->name))
    {

        Operand* label1 = newLabel();
        translate_Cond(node->child, label1, label_false);
        singleCode(LABEL_I, label1);       
        if(stmtCount == 8)
        {
            condCount++;
        }
        translate_Cond(node->child->bro->bro, label_true, label_false);
 
    } 
     // Exp OR Exp
     else if(node->child->bro&&!strcmp("OR",node->child->bro->name))
    {

        Operand* label1 = newLabel();
        translate_Cond(node->child, label_true, label1);
        singleCode(LABEL_I, label1);
        translate_Cond(node->child->bro->bro, label_true, label_false);
    } 
    // Exp RELOP Exp
    else if(node->child->bro&&!strcmp("RELOP",node->child->bro->name))
    {

        Operand* t1 = newTemp(VAL_O);
        Operand* t2 = newTemp(VAL_O);
        translate_Exp(node->child, t1,0);
        translate_Exp(node->child->bro->bro, t2,0);
        condCode(t1, t2, label_true, node->child->bro->s_val);
        singleCode(GOTO_I, label_false);
    } 
    // NOT Exp
    else if(node->child&&!strcmp("NOT",node->child->name))
    {


        translate_Cond(node->child->bro,label_false, label_true);
    }
    else {
        Operand* t1 = newTemp(VAL_O);
        translate_Exp(node, t1,0);
        condCode(t1, newConstant(0), label_true, "!=");
        singleCode(GOTO_I, label_false);
    } 
}

//headType =1???????????????????????????????????????0
Type* translate_Exp(treeNode* node,Operand* place,int headType)
{
    if (node==NULL)
            return NULL;
    //Exp->INT
     if(!strcmp("INT",node->child->name))
        {

            if(place!=NULL)
            {
                Operand* op = (Operand*)malloc(sizeof(Operand));
                op->kind = CONSTANT_O;
                op->u.value = node->child->i_val;
                assignCode(place, op,NORMAL_I);
            }
        }
    //Exp->FLOAT
    else if(!strcmp("FLOAT",node->child->name))
        {
            //????????????
        }
    else if(!strcmp("Exp",node->child->name))
        {

            if(node->child->bro==NULL)
                return NULL ;
            //Exp -> Exp ASSIGNOP Exp
            if(!strcmp("ASSIGNOP",node->child->bro->name))
            {
                if(DEBUG)
                {
                    printf("?????? ??????%d\n",node->line);
                }
                treeNode* E1 = node->child;
                treeNode* E2 = node->child->bro->bro;
                //E1->ID 
                if(E1->child&&!strcmp("ID",E1->child->name)&&!E1->child->bro)
                {
                    hashNode* symbol = search(E1->child->s_val);
                    if(!symbol)
                        exit(-1);
                    Operand* t1 = symbol->op;
                    Operand* t2;
                    //???????????????????????????????????????????????????
                    if(t1->type==ADDRESS_O&&symbol->type->kind==STRUCTURE)
                        {
                            printf("direct assign on  structure\n");
                            //return NULL;
                            exit(-1);
                        }
                    else if(t1->type==ADDRESS_O&&symbol->type->kind==ARRAY)
                    {
                            //????????????
                            Operand* t3 = newTemp(ADDRESS_O);
                            //????????????????????????????????????????????????
                            if(t1->isfunctionpara==0)
                                assignCode(t3,t1,GETADDR_I);
                            else    
                                assignCode(t3,t1,NORMAL_I);
                            t2 = newTemp(ADDRESS_O);
                            Type* E2type = translate_Exp(E2,t2,0);
                            //??????COPY?????????
                            arrayAssign(symbol->type,E2type,t3,t2);
                    }
                    else
                    {
                        t2 = newTemp(VAL_O);
                        translate_Exp(E2,t2,0);
                        assignCode(t1 , t2,NORMAL_I);
                    }
                    if (place) 
                        assignCode( place, t1,NORMAL_I);
                }
                else if(E1->child&&!strcmp("Exp",E1->child->name))
                {
                    // E1->Exp LB Exp RB 
                    if(E1->child->bro&&!strcmp("LB",E1->child->bro->name))
                    {
                        //E1????????????????????????????????????t1
                        Operand* t1= newTemp(ADDRESS_O);
                        Type* E1type = translate_Exp(E1,t1,0);
                        if(E1type)
                            {
                                //??????????????????
                                if(E1type->kind==ARRAY)
                                {
                                    Operand* t2 = newTemp(ADDRESS_O);
                                    Type* E2type = translate_Exp(E2,t2,0);
                                    arrayAssign(E1type,E2type,t1,t2);
                                }
                                else
                                {
                                       Operand* t2 = newTemp(VAL_O);
                                        translate_Exp(E2,t2,0);
                                        assignCode(t1,t2,SETVAL_I);
                                }
                            }
                        if (place) 
                        {
                            assignCode( place, t1,NORMAL_I);
                        }

                    }
                    //E1->Exp DOT ID
                    else if(E1->child->bro&&!strcmp("DOT",E1->child->bro->name))
                    {
                            //E1??????????????????????????????????????????t1
                            Operand* t1 = newTemp(ADDRESS_O);
                            Type* E1type = translate_Exp(E1, t1,headType);
                            if(E1type&&E1type->kind==ARRAY)
                                {
                                //??????????????????
                                    Operand* t2 = newTemp(ADDRESS_O);
                                    Type* E2type = translate_Exp(E2,t2,0);
                                    arrayAssign(E1type,E2type,t1,t2);
                        
                                }        
                            else
                                {
                                       Operand* t2 = newTemp(VAL_O);
                                        translate_Exp(E2,t2,0);
                                        assignCode(t1,t2,SETVAL_I);
                                }
                            if (place) 
                                assignCode( place, t1,NORMAL_I);
                    }
                }
            }
            //Exp -> Exp LB Exp RB
            else if(!strcmp("LB",node->child->bro->name))
            {
                //??????????????????place?????????????????????????????????
                if(place)
                {
                            if(DEBUG)
                        {
                            printf("???????????? ?????????%d  \n",node->line);
                        }
                    Operand* t1 = newTemp(ADDRESS_O);
                    //??????E1??????????????????
                    Type*typeE1 = translate_Exp(node->child,t1,headType);   
                    if(DEBUG)
                        {
                            printf("???????????? ?????????%d ??????????????????%d ????????? \n",node->line,typeE1->kind);
                        }
                    if(!typeE1)
                        exit(-1);
                    Type*typeElement = typeE1->u.array.elem;
                    int ElementSize = getTypeSize(typeElement);
                    Operand*t2 = newTemp(VAL_O);
                    //??????E2??????
                    translate_Exp(node->child->bro->bro,t2,headType);
                    //???????????????????????????????????????????????????E1???????????????
                    Operand* offset = newTemp(VAL_O);
                    binCode(MUL_I, offset, t2, newConstant(ElementSize));
                    //????????????????????????
                    if(place->type==VAL_O)
                    {
                        Operand* add = newTemp(ADDRESS_O);
                        binCode(ADD_I, add,t1,offset);
                        assignCode(place, add,GETVAL_I);
                    }
                    //???????????????????????????
                    else
                    {
                        binCode(ADD_I, place,t1,offset);
                    }
                    return typeElement;
                }
            }
            //Exp -> Exp DOT ID
            else if(!strcmp("DOT",node->child->bro->name))
            {
                if(place)
                {
                    if(DEBUG)
                    {
                        printf("??????????????? ?????????%d\n",node->line);
                        printf("?????????????????????%s\n",node->child->bro->bro->s_val);
                    }
                    Operand* t1 = newTemp(ADDRESS_O);
                    Type* type = translate_Exp(node->child,t1,headType);  //???????????????????????????t1,?????????E1???type
                    
                    if(type)
                    {
                        if(type->kind==STRUCTURE)
                        {
                            //?????????????????????ID??????????????????????????????
                            Type* returnType;
                            int offset = getOffsetInStruct(type,node->child->bro->bro->s_val,&returnType);
                                if(DEBUG)
                                    printf("getType:%d\n",returnType->kind);
                            Operand* off = newConstant(offset);
                            //???????????????
                            if(place->type==VAL_O)
                            {
                                Operand* add = newTemp(ADDRESS_O);
                                binCode(ADD_I, add, t1, off);
                                assignCode( place, add,GETVAL_I);
                            }
                            //??????????????????
                            else
                            {
                                binCode(ADD_I, place, t1, off);
                            }                   
                            return returnType;          
                        }
                    }
                }

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
                            assignCode(place, newConstant(0),NORMAL_I);
                        }
                    translate_Cond(node, label1, label2);
                    singleCode(LABEL_I, label1);
                    if (place)
                        {
                            assignCode(place,newConstant(1),NORMAL_I);
                        }
                    singleCode(LABEL_I, label2);
                }
                else if(!strcmp("PLUS",node->child->bro->name)||!strcmp("MINUS",node->child->bro->name)||!strcmp("STAR",node->child->bro->name)||!strcmp("DIV",node->child->bro->name))
                {
                        if(DEBUG)
                        {
                            printf("???????????? ?????????%d\n",node->line);
                        }
                        treeNode* E1 = node->child;
                        treeNode* E2 = node->child->bro->bro;
                        Operand* t1 = newTemp(VAL_O);
                        translate_Exp(E1,t1,0);
                        
                        Operand* t2 = newTemp(VAL_O);
                        translate_Exp(E2,t2,0);
                        if (place)
                        {
                            if(!strcmp("PLUS",node->child->bro->name))
                                binCode(ADD_I, place, t1, t2);
                            if(!strcmp("MINUS",node->child->bro->name))
                                binCode(SUB_I, place, t1, t2);
                            if(!strcmp("STAR",node->child->bro->name))
                                binCode(MUL_I, place, t1, t2);
                            if(!strcmp("DIV",node->child->bro->name))
                                binCode(DIV_I, place, t1, t2);
                        }
                    }
            }
        }
    //EXP-> LP Exp RP | MINUS EXP
    else if(!strcmp("LP",node->child->name)||!strcmp("MINUS",node->child->name))
        {

            if(!strcmp("LP",node->child->name))
                translate_Exp(node->child->bro, place,0);
            else if(!strcmp("MINUS",node->child->name))
            {
                Operand* t1 = newTemp(VAL_O);
                translate_Exp(node->child->bro, t1,0);
                if (place)
                    binCode(SUB_I, place, newConstant(0), t1);
            }
        } 
    //EXP-> NOT EXP
    else if(!strcmp("NOT",node->child->name))
        {
                if(condCount!=0)
                    ++condCount;
                Operand* label1 = newLabel();
                Operand* label2 = newLabel();
                if (place)
                    {
                        assignCode(place, newConstant(0),NORMAL_I);
                    }                              

                translate_Cond(node, label1, label2);
                singleCode(LABEL_I, label1);
                if (place)
                    {
                        assignCode(place,newConstant(1),NORMAL_I);
                    }
                singleCode(LABEL_I, label2);

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
                if(!symbol)
                    exit(-1);
                if(place->type==VAL_O)
                {
                    //???????????????????????????????????????????????????????????????
                    assignCode(place, symbol->op,NORMAL_I);
                }
                else
                {
                    //????????????????????????
                    if(symbol->op->type==VAL_O) //?????????????????????
                        assignCode(place, symbol->op,GETADDR_I);
                    else    //???????????????
                    {
                        //???????????????????????????????????????
                        if(symbol->op->isfunctionpara==1)
                            assignCode(place, symbol->op,NORMAL_I);
                        else
                            assignCode(place, symbol->op,GETADDR_I);
                    }
                }
                return symbol->type;
            }
        }
        //ID LP Args RP
        else if(!strcmp("Args",node->child->bro->bro->name))
        {

            hashNode* func = search(node->child->s_val);
            if(!func)
                exit(-1);
            int arg_count = func->type->u.function.argCount;
            Operand * arg_list[arg_count];
            translate_Args(node->child->bro->bro,arg_list,arg_count-1,func->type->u.function.para);
            if (!strcmp(func->name, "write")) 
            {
                singleCode(WRITE_I, arg_list[0]);
                if (place)
                    assignCode(place, newConstant(0),NORMAL_I);
            } 
            else {
                for (int i = 0; i < arg_count; ++i) {
                    singleCode(ARG_I, arg_list[i]);
                }
                Operand* op = (Operand*)malloc(sizeof(Operand));
                op->kind = FUNCTION_O;
                op->u.func_name = func->name;
                if (place)
                    unaryCode(CALL_I, place, op);
                else {
                    Operand* t1 = newTemp(VAL_O);
                    unaryCode(CALL_I, t1, op);
                }
            }
        }
    //EXP-> ID LP RP
    else
        {
            hashNode* func = search(node->child->s_val);
            if(!strcmp(func->name,"read"))
            {
                if (place)
                        singleCode(READ_I, place);
                else 
                {
                    Operand* t1 = newTemp(VAL_O);
                    singleCode(READ_I, t1);
                }
            }
            else
            {
                Operand* op = (Operand*)malloc(sizeof(Operand));
                op->kind = FUNCTION_O;
                op->u.func_name = func->name;
                if (place)
                    unaryCode(CALL_I, place, op);
                else {
                    Operand* t1 = newTemp(VAL_O);
                    unaryCode(CALL_I, t1, op);
                }
            }
        }
    }
    return NULL;
}

void  translate_Args(treeNode* node,Operand* arg_list[],int head,FieldList* arghead)
{
    if(node==NULL)
        return;
        // :Exp COMMA Args
    if(node->child->bro!=NULL)
    {
        Operand* t1;
        if(!arghead)
            exit(-1);
        if(arghead->type->kind==BASIC)
            t1=newTemp(VAL_O);
        else
            t1=newTemp(ADDRESS_O);
        translate_Exp(node->child,t1,1);
        arg_list[head]=t1;
        head--;
        translate_Args(node->child->bro->bro,arg_list,head,arghead->tail);
    }
    //:Exp
    else
    {
        Operand* t1;
        if(arghead->type->kind==BASIC)
            t1=newTemp(VAL_O);
        else
            t1=newTemp(ADDRESS_O);
        translate_Exp(node->child,t1,1);
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
    singleCode(FUNCTION_I, func);
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

//0????????????1??????????????????2???????????????
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

//name[INT][INT]
void translate_VarDec(treeNode* node,int headType)
{
    if (node==NULL)
        return ;
        //VarDec -> ID
        if(!strcmp("ID",node->child->name))
        { 
            hashNode* sym = search(node->child->s_val);
            if(!sym)
                exit(-1);
            //????????????
            if (headType==1) 
            {
                sym->op->kind = PARAMETER_O;
                singleCode(PARAM_I, sym->op);
            }
            //?????????
            else if(headType==2)
            {
                decCode(sym->op, sym->size);
            }
            else
            {
                if(sym->type->kind==ARRAY)
                    decCode(sym->op, sym->size);
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

//???????????????2???????????????0
int translate_Specifier(treeNode* node)
{
    if (node==NULL)
             return 0;
    //Specifier -> TYPE
    if(!strcmp( "TYPE",node->child->name))  
    {
        return 0;
    }
    //Specifier -> StructSpecifier
    else    
        return 2;
}

void translate_Def(treeNode* node)
{
    if (node==NULL)
        return;
    //Def -> Specifier DecList SEMI
    if(node->child)
    {
        int headType = translate_Specifier(node->child);
        translate_DecList(node->child->bro,headType);
    }
}

void translate_DecList(treeNode *node,int headType)
 {
    if (node==NULL)
        return ;
    //DecList -> Dec
    if(node->child->bro == NULL) 
    {
       translate_Dec(node->child,headType);
    }
    //DecList -> Dec COMMA DecList
    else   
    {
        translate_Dec(node->child,headType);
        translate_DecList(node->child->bro->bro,headType);
    }
}

void  translate_Dec(treeNode *node,int headType) 
{
    if (node==NULL)
        return ;
    //Dec ->VarDec
    if(node->child->bro == NULL)
    {
        translate_VarDec(node->child,headType);
    }
    //Dec -> VarDec ASSIGNOP Exp
    else   
    {
        
        treeNode* cur = node;
        while(cur->child)   //??????VarDec?????????
            cur=cur->child;
        hashNode* left = search(cur->s_val);
        if(!left)
            exit(-1);
        if(left->type->kind == ARRAY)
        {
            //????????????
            exit(1);
        }
        else
        {
        Operand* t1 = left->op;
        translate_VarDec(node->child,headType);
        Operand* t2 = newTemp(VAL_O);
        translate_Exp(node->child->bro->bro, t2,0);
        assignCode(t1, t2,NORMAL_I);
        }
    }
}