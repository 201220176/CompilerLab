#include"IR.h"

int varCount = 1, tempCount = 1, labelCount = 1;
InterCodes* head;
InterCodes* tail;


char* printOperand(Operand* op)
{
    char* res = (char*)malloc(40);
    switch (op->kind) {
    case VARIABLE_O:
        sprintf(res, "v%d", op->u.var_no);
        break;
    case TEMP_O:
        sprintf(res, "t%d", op->u.var_no);
        break;
    case PARAMETER_O:
        sprintf(res, "v%d", op->u.var_no);
        break;
    case CONSTANT_O:
        sprintf(res, "#%lld", op->u.value);
        break;
    case LABEL_O:
        sprintf(res, "label%d", op->u.var_no);
        break;
    case FUNCTION_O:
        sprintf(res, "%s", op->u.func_name);
        break;
    default:
        break;
    }
    return res;
}

void writeToFile(char* fielname)
{
    FILE* f =fopen(fielname,"w");
    InterCodes* cur = head->next;
    while(cur)
    {
        switch (cur->code.kind) {
            case LABEL_I:
                fprintf(f, "LABEL %s :\n", printOperand(cur->code.u.single.op));
                break;
            case FUNCTION_I:
                fprintf(f, "FUNCTION %s :\n", printOperand(cur->code.u.single.op));
                break;
            case ASSIGN_I: {
                    char* l = printOperand(cur->code.u.assign.left);
                    char* r = printOperand(cur->code.u.assign.right);
                    switch (cur->code.type) {
                    case NORMAL_I:
                        fprintf(f, "%s := %s\n", l, r);
                        break;
                    case GETVAL_I:
                        fprintf(f, "%s := *%s\n", l, r);
                        break;
                    case SETVAL_I:
                        fprintf(f, "*%s := %s\n", l, r);
                        break;
                    default:
                        break;
                    }
                } break;
            case ADD_I: {
                char* r = printOperand(cur->code.u.binop.res);
                char* op1 = printOperand(cur->code.u.binop.op1);
                char* op2 = printOperand(cur->code.u.binop.op2);
                fprintf(f, "%s := %s + %s\n", r, op1, op2);
            } break;
            case SUB_I: {
                char* r = printOperand(cur->code.u.binop.res);
                char* op1 = printOperand(cur->code.u.binop.op1);
                char* op2 = printOperand(cur->code.u.binop.op2);
                fprintf(f, "%s := %s - %s\n", r, op1, op2);
            } break;
            case MUL_I: {
                char* r = printOperand(cur->code.u.binop.res);
                char* op1 = printOperand(cur->code.u.binop.op1);
                char* op2 = printOperand(cur->code.u.binop.op2);
                fprintf(f, "%s := %s * %s\n", r, op1, op2);
            } break;
            case DIV_I: {
                char* r = printOperand(cur->code.u.binop.res);
                char* op1 = printOperand(cur->code.u.binop.op1);
                char* op2 = printOperand(cur->code.u.binop.op2);
                fprintf(f, "%s := %s / %s\n", r, op1, op2);
            } break;
            case GOTO_I:
                fprintf(f, "GOTO %s\n", printOperand(cur->code.u.single.op));
                break;
            case IF_I: {
                char* op1 = printOperand(cur->code.u.cond.op1);
                char* op2 = printOperand(cur->code.u.cond.op2);
                char* tar = printOperand(cur->code.u.cond.target);
                fprintf(f, "IF %s %s %s GOTO %s\n", op1, cur->code.u.cond.relop, op2, tar);
            } break;
            case RETURN_I:
                fprintf(f, "RETURN %s\n", printOperand(cur->code.u.single.op));
                break;
            case DEC_I:
                fprintf(f, "DEC %s %u\n", printOperand(cur->code.u.dec.op), cur->code.u.dec.size);
                break;
            case ARG_I:
                fprintf(f, "ARG %s\n", printOperand(cur->code.u.single.op));
                break;
            case CALL_I: {
                char* res = printOperand(cur->code.u.sinop.res);
                char* op = printOperand(cur->code.u.sinop.op);
                fprintf(f, "%s := CALL %s\n", res, op);
            } break;
            case PARAM_I:
                fprintf(f, "PARAM %s\n", printOperand(cur->code.u.single.op));
                break;
            case READ_I:
                fprintf(f, "READ %s\n", printOperand(cur->code.u.single.op));
                break;
            case WRITE_I:
                    fprintf(f, "WRITE %s\n", printOperand(cur->code.u.single.op));
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
    op->u.var_no = labelCount;
    ++labelCount;
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
    op->u.var_no = tempCount;
    ++tempCount;
    return op;
}

Operand *newConstant(int val)
{
    Operand* op = (Operand*)malloc(sizeof(Operand));
    op->kind = CONSTANT_O;
    op->u.value = val;
    return op;
}

void createSingle(int kind,Operand*op)
{
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = kind;
    p->code.u.single.op = op;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}

void createCond(Operand* op1, Operand* op2, Operand* target, char* re)
{
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = IF_I;
    p->code.u.cond.op1 = op1;
    p->code.u.cond.op2 = op2;
    p->code.u.cond.target = target;
    strcpy(p->code.u.cond.relop, re);
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}

void createAssign( Operand* left, Operand* right,int type)
{
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = ASSIGN_I;
    p->code.type = type;
    p->code.u.assign.left = left;
    p->code.u.assign.right = right;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}

void createBinop(unsigned kind,  Operand* res, Operand* op1, Operand* op2)
{
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = kind;
    p->code.u.binop.res = res;
    p->code.u.binop.op1 = op1;
    p->code.u.binop.op2 = op2;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}

void createSinop(unsigned kind, Operand* res, Operand* op)
{
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = kind;
    p->code.u.sinop.res = res;
    p->code.u.sinop.op = op;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}

void createDec(Operand* op, unsigned size)
{
    InterCodes* p = (InterCodes*)malloc(sizeof(InterCodes));
    p->code.kind = DEC_I;
    p->code.u.dec.op = op;
    p->code.u.dec.size = size;
    p->next = NULL;
    p->prev = tail;
    tail->next = p;
    tail = tail->next;
}

void translate_init()
{
    head = (InterCodes*)malloc(sizeof(InterCodes));
    head->prev = head->next = NULL;
    tail = head;
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

int getOffsetInStruct(Type* type,char* domain,Type* returnType)
{
    if(!type)
    {
        returnType=NULL;
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
    return res;
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
                Operand* t1 = newTemp(VAL_O);
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
    if(node->child->bro&&!strcmp("AND",node->child->bro->name))
    {
        Operand* label1 = newLabel();
        translate_Cond(node->child, label1, label_false);
        createSingle(LABEL_I, label1);
        translate_Cond(node->child->bro->bro, label_true, label_false);
    } 
     // Exp OR Exp
     else if(node->child->bro&&!strcmp("OR",node->child->bro->name))
    {
        Operand* label1 = newLabel();
        translate_Cond(node->child, label_true, label1);
        createSingle(LABEL_I, label1);
        translate_Cond(node->child->bro->bro, label_true, label_false);
    } 
    // Exp RELOP Exp
    else if(node->child->bro&&!strcmp("RELOP",node->child->bro->name))
    {
        Operand* t1 = newTemp(VAL_O);
        Operand* t2 = newTemp(VAL_O);
        translate_Exp(node->child, t1);
        translate_Exp(node->child->bro->bro, t2);
        createCond(t1, t2, label_true, node->child->bro->s_val);
        createSingle(GOTO_I, label_false);
    } 
    // NOT Exp
    else if(node->child&&!strcmp("NOT",node->child->s_val))
        translate_Cond(node->child->bro,label_false, label_true);
    else {
        Operand* t1 = newTemp(VAL_O);
        translate_Exp(node, t1);
        createCond(t1, newConstant(0), label_true, "!=");
        createSingle(GOTO_I, label_false);
    } 
}

Type* translate_Exp(treeNode* node,Operand* place)
{
    if (node==NULL)
            return NULL ;
    //Exp->INT
     if(!strcmp("INT",node->child->name))
        {
            if(place!=NULL)
            {
                Operand* op = (Operand*)malloc(sizeof(Operand));
                op->kind = CONSTANT_O;
                op->u.value = node->child->i_val;
                createAssign(place, op,NORMAL_I);
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
                return NULL ;
            //Exp -> Exp ASSIGNOP Exp
            if(!strcmp("ASSIGNOP",node->child->bro->name))
            {
                treeNode* E1 = node->child;
                treeNode* E2 = node->child->bro->bro;
                //E1->ID 
                if(E1->child&&!strcmp("ID",E1->child->name)&&!E1->child->bro)
                {
                    hashNode* symbol = search(E1->child->s_val);
                    Operand* t1 = symbol->op;
                    Operand* t2;
                    //左边为地址，那就为结构体或数组，都是不允许的
                    if(t1->type==ADDRESS_O)
                        {
                            printf("direct assign on array or structure\n");
                            exit(-1);
                        }
                    else
                        t2 = newTemp(VAL_O);
                    translate_Exp(E2,t2);
                    createAssign(t1 , t2,NORMAL_I);
                    if (place) 
                        createAssign( place, t1,NORMAL_I);
                }
                else if(E1->child&&!strcmp("Exp",E1->child->name))
                {
                    // E1->Exp LB Exp RB 
                    if(E1->child->bro&&!strcmp("LB",E1->child->bro->name))
                    {
                        //E1是数组访问，则获取地址于t1
                        Operand* t1= newTemp(ADDRESS_O);
                        translate_Exp(E1,t1);
                        Operand* t2 = newTemp(VAL_O);
                        translate_Exp(E2,t2);
                        createAssign(t1,t2,SETVAL_I);
                        if (place) 
                            createAssign( place, t1,NORMAL_I);

                    }
                    //E1->Exp DOT ID
                    else if(E1->child->bro&&!strcmp("DOT",E1->child->bro->name))
                    {
                            //E1是结构体中元素，则获取地址于t1
                            Operand* t1 = newTemp(ADDRESS_O);
                            translate_Exp(E1, t1);
                            //E2还是应该是某个值
                            Operand* t2 = newTemp(VAL_O);
                            translate_Exp(E2, t2);
                            createAssign(t1, t2,SETVAL_I);
                            if (place) 
                                createAssign( place, t1,NORMAL_I);
                    }
                }
            }
            //Exp -> Exp LB Exp RB
            else if(!strcmp("LB",node->child->bro->name))
            {
                //根据传进来的place判断应该返回值还是地址
                if(place)
                {
                    Operand* t1 = newTemp(ADDRESS_O);
                    //返回E1的地址和类型
                    Type*typeE1 = translate_Exp(node->child,t1);
                    Type*typeElement = typeE1->u.array.elem;
                    int ElementSize = getTypeSize(typeElement);

                    Operand*t2 = newTemp(VAL_O);
                    //返回E2的值
                    translate_Exp(node->child->bro->bro,t2);
                    //计算当前维数的偏移量，等会儿再加上E1的地址即可
                    Operand* offset = newTemp(VAL_O);
                    createBinop(MUL_I, offset, t2, newConstant(ElementSize));
                    //渴望得到该处的值
                    if(place->type==VAL_O)
                    {
                        Operand* add = newTemp(ADDRESS_O);
                        createBinop(ADD_I, add,t1,offset);
                        createAssign(place, add,GETVAL_I);
                    }
                    //渴望得到该处的地址
                    else
                    {
                        createBinop(ADD_I, place,t1,offset);
                    }
                    return typeElement;
                }
            }
            //Exp -> Exp DOT ID
            else if(!strcmp("DOT",node->child->bro->name))
            {
                if(place)
                {
                    Operand* t1 = newTemp(ADDRESS_O);
                    Type* type = translate_Exp(node->child,t1);  //应该会返回一个地址t1,并返回E1的type
                    if(type)
                    {
                        if(type->kind==STRUCTURE)
                        {
                            //从结构体中获取ID的类型和所处偏移量。
                            Type* returnType;
                            int offset = getOffsetInStruct(type,node->child->bro->bro->s_val,returnType);
                            Operand* off = newConstant(offset);
                            //渴望获取值
                            if(place->type==VAL_O)
                            {
                                Operand* add = newTemp(ADDRESS_O);
                                createBinop(ADD_I, add, t1, off);
                                createAssign( place, add,GETVAL_I);
                            }
                            //渴望获取地址
                            else
                            {
                                createBinop(ADD_I, place, t1, off);
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
                            createAssign(place, newConstant(0),NORMAL_I);
                        }
                    translate_Cond(node, label1, label2);
                    createSingle(LABEL_I, label1);
                    if (place)
                        {
                            createAssign(place,newConstant(1),NORMAL_I);
                        }
                    createSingle(LABEL_I, label2);
                }
                else if(!strcmp("PLUS",node->child->bro->name)||!strcmp("MINUS",node->child->bro->name)||!strcmp("STAR",node->child->bro->name)||!strcmp("DIV",node->child->bro->name))
                {
                        treeNode* E1 = node->child;
                        treeNode* E2 = node->child->bro->bro;
                        Operand* t1 = newTemp(VAL_O);
                        translate_Exp(E1,t1);
                        Operand* t2 = newTemp(VAL_O);
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
                Operand* t1 = newTemp(VAL_O);
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
                        createAssign(place, newConstant(0),NORMAL_I);
                    }
                translate_Cond(node, label1, label2);
                createSingle(LABEL_I, label1);
                if (place)
                    {
                        createAssign(place,newConstant(1),NORMAL_I);
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
                if(place->type==VAL_O)
                {
                    //既然希望得到一个值，那么必然只能为基本类型
                    createAssign(place, symbol->op,NORMAL_I);
                }
                else
                {
                    //既然希望得到一个地址，那本身这个符号就是地址
                    createAssign(place, symbol->op,NORMAL_I);
                }
                return symbol->type;
            }
        }
        //ID LP Args RP
        else if(!strcmp("Args",node->child->bro->bro->name))
        {
            hashNode* func = search(node->child->s_val);
            int arg_count = func->type->u.function.argCount;
            Operand * arg_list[arg_count];
            translate_Args(node->child->bro->bro,arg_list,arg_count-1,func->type->u.function.para);
            if (!strcmp(func->name, "write")) 
            {
                createSingle(WRITE_I, arg_list[0]);
                if (place)
                    createAssign(place, newConstant(0),NORMAL_I);
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
                    Operand* t1 = newTemp(VAL_O);
                    createSinop(CALL_I, t1, op);
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
                        createSingle(READ_I, place);
                else 
                {
                    Operand* t1 = newTemp(VAL_O);
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
                    Operand* t1 = newTemp(VAL_O);
                    createSinop(CALL_I, t1, op);
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
        if(arghead->type->kind==BASIC)
            t1=newTemp(VAL_O);
        else
            t1=newTemp(ADDRESS_O);
        translate_Exp(node->child,t1);
        arg_list[head]=t1;
        translate_Args(node->child->bro->bro,arg_list,--head,arghead->tail);
    }
    //:Exp
    else
    {
        Operand* t1;
        if(arghead->type->kind==BASIC)
            t1=newTemp(VAL_O);
        else
            t1=newTemp(ADDRESS_O);
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

//name[INT][INT]
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
                hashNode* symbol = search(node->child->s_val);
                createDec(symbol->op, symbol->size);
            }
            else
            {
                hashNode* symbol = search(node->child->s_val);
                if(symbol->type->kind==ARRAY)
                    createDec(symbol->op, symbol->size);
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
        Operand* t1 = newTemp(VAL_O);
        translate_Exp(node->child->bro->bro, t1);
        createAssign(res->op, t1,NORMAL_I);
    }
}