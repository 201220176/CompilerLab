#ifndef IR_H
#define IR_H

#define DEBUG 0
#include"semantic.h"

typedef struct Operand_ Operand;
typedef struct InterCode InterCode;
typedef struct InterCodes InterCodes;

struct Operand_ {
    enum { VARIABLE_O, TEMP_O, PARAMETER_O,FUNCTION_O,CONSTANT_O, LABEL_O } kind;
    enum { VAL_O, ADDRESS_O} type;
    int isfunctionpara;     //仅当该符号出现在符号表中，被判断为变量时使用，因为当其出现在函数参数时，值为真正的地址；而局部变量则只为名字。
    union {
        int var_no;
        int value;
        char* func_name;
    } u;
};

struct InterCode {
    enum { ASSIGN_I, ADD_I, SUB_I, MUL_I,  READ_I, WRITE_I,DIV_I, LABEL_I, FUNCTION_I,  PARAM_I,  CALL_I,ARG_I,IF_I,GOTO_I,  RETURN_I, DEC_I} kind;
    union {
        struct {
            Operand *left, *right;
            enum { NORMAL_I, GETVAL_I, SETVAL_I,GETADDR_I,COPY_I} type;
        } assign;
        struct {
            Operand *res, *op1, *op2;
        } binop;
        struct {
            Operand *res, *op;
        } unaryop;
        struct {
            Operand* op;
        } single;        
        struct {
            Operand* op;
            int size;
        } dec;
        struct {
            Operand *op1, *op2, *target;
            char *relop;
        } cond;
    } u;
};

struct InterCodes {
    InterCode code;
    InterCodes *prev, *next;
};

char* toOp(Operand* op);
void writeToFile(char* fielname);
Operand* newLabel();
Operand* newTemp(int type);
Operand *newConstant(int val);
void singleCode(int,Operand*);
void condCode(Operand* op1, Operand* op2, Operand* target, char* re);
void assignCode( Operand* left, Operand* right,int type);
void binCode(unsigned kind,  Operand* res, Operand* op1, Operand* op2);
void unaryCode(unsigned kind, Operand* res, Operand* op);
void decCode(Operand* op, unsigned size);
void translate_init();
Operand* new_symbol_op(char*name,Type* type,int isfunpara);
int getOffsetInStruct(Type* type,char* domain,Type** returnType);
void Translate(treeNode *root);
void translate_Program(treeNode* node);
void translate_ExtDefList(treeNode* node);
void translate_ExtDef(treeNode* node);
void translate_CompSt(treeNode* node) ;
void translate_StmtList(treeNode* node);
void translate_Stmt(treeNode* node);
void translate_Cond(treeNode* node, Operand* label_true, Operand* label_false);
Type* translate_Exp(treeNode* node,Operand* place , int headType);
void  translate_Args(treeNode* node,Operand* arg_list[],int head,FieldList* arghead);
void translate_FunDec(treeNode* node);
void translate_VarList(treeNode* node,int headType);
void translate_ParamDec(treeNode* node,int headType);
void translate_ExtDecList(treeNode* node);
void translate_VarDec(treeNode* node,int headType);
void translate_DefList(treeNode* node);
int translate_Specifier(treeNode* node);
void translate_Def(treeNode* node);
void translate_DecList(treeNode *node,int headType);
void  translate_Dec(treeNode *node,int headType) ;


#endif