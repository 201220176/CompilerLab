#ifndef IR_H
#define IR_H
#include"semantic.h"

typedef struct Operand_ Operand;
typedef struct InterCode InterCode;
typedef struct InterCodes InterCodes;

struct Operand_ {
    enum { VARIABLE_O, TEMP_O, PARAMETER_O,FUNCTION_O,CONSTANT_O, LABEL_O } kind;
    enum { VAL_O, ADDRESS_O} type;
    union {
        int var_no;
        long long int value;
        char* func_name;
    } u;
};

struct InterCode {
    enum { LABEL_I, ASSIGN_I, ADD_I,FUNCTION_I,  SUB_I, MUL_I, DIV_I, GOTO_I, IF_I, RETURN_I, DEC_I, ARG_I, CALL_I, PARAM_I, READ_I, WRITE_I } kind;
    enum { NORMAL_I, GETADDR_I, GETVAL_I, SETVAL_I, COPY_I } type;
    union {
        struct {
            Operand *left, *right;
        } assign;
        struct {
            Operand *res, *op1, *op2;
        } binop;
        struct {
            Operand *res, *op;
        } sinop;
        struct {
            Operand* op;
        } single;
        struct {
            Operand *op1, *op2, *target;
            char relop[4];
        } cond;
        struct {
            Operand* op;
            unsigned size;
        } dec;
    } u;
};

struct InterCodes {
    InterCode code;
    int isDelete;
    InterCodes *prev, *next;
};

void writeToFile(char* fielname);
Operand* newLabel();
Operand* newTemp();
void createSingle(int,Operand*);
void translate_init();
Operand* new_symbol_op(char*name,Type* type);
void Translate(treeNode *root);
void translate_Program(treeNode* node);
void translate_ExtDefList(treeNode* node);
void translate_ExtDef(treeNode* node);
void translate_CompSt(treeNode* node) ;
void translate_StmtList(treeNode* node);
void translate_Stmt(treeNode* node);
void translate_Cond(treeNode* node, Operand* label_true, Operand* label_false);
void translate_Exp(treeNode* node,Operand* place);
void  translate_Args(treeNode* node,Operand* arg_list[],int head);
void translate_FunDec(treeNode* node);
void translate_VarList(treeNode* node,int headType);
void translate_ParamDec(treeNode* node,int headType);
void translate_ExtDecList(treeNode* node);
void translate_VarDec(treeNode* node,int headType);
void translate_DefList(treeNode* node);
void translate_Def(treeNode* node);
void translate_DecList(treeNode *node);
void translate_Dec(treeNode *node) ;


#endif