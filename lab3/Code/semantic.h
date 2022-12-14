#ifndef SEMANTIC_H 

#define SEMANTIC_H
#include"tree.h"
#include <stdio.h>
#include <string.h>

typedef struct Operand_ Operand;

#define HASHSIZE    0x3fff
enum {
    Type0,Type1,Type2,Type3,Type4,Type5,
    Type6,Type7,Type8,Type9,Type10,
    Type11,Type12,Type13,Type14,Type15,
    Type16,Type17,Type18,Type19
};

typedef struct Type{
    enum { BASIC, ARRAY, STRUCTURE,FUNCTION} kind;
    union {
        //基本类型
        int  basic;
        //数组类型信息包括元素类型与数组大小构成
        struct {struct Type* elem; int size; } array;
        //结构体类型信息是一个链表
        struct FieldList* structure;
        struct { struct Type* ret; struct FieldList* para;int argCount; } function;
    } u;
    //判断是否是变量，还是说是结构体的名字。
    int is_var;
}Type;

typedef struct FieldList {
    char* name; //域的名字
    Type* type; //域的类型
    struct FieldList* tail; //下一个域
}FieldList;

typedef struct hashNode{
    char* name;
    Type* type;
    Operand * op;   //在表中存储其操作数。
    int line;
    int defined;
    int size;
    int depth;
    //opening hashing
    struct hashNode * next;
    //下一个同层的Hashnode
    struct hashNode * under;
}hashNode;


void tableInit();
unsigned hash_pjw(char* name);
void printTable();
Type* getTypeFromTable( char * name);
Type* getTypeFromStruct( char * name,Type* structure);
void DeleteInTable(hashNode* node);
void removeNodes(int depth);
void insert(hashNode* node);
hashNode* search( char * name);
hashNode* newSymbol( char* name, Type* type, int line, int defined,int depth,int isfunpara);
void addSymbol( char* name, Type* type, int line, int defined,int depth,int isfunpara);
int isDoInStructure( char* domain,Type*structure);
void checkDeclarationAndDefine();
void Program(treeNode* node);
void ExtDefList(treeNode* node);
void ExtDef(treeNode* node);
void CompSt(treeNode* node, Type* ret, int defined,int depth) ;
void StmtList(treeNode* node, Type* ret,int depth);
void Stmt(treeNode* node, Type* ret,int depth);
Type* Exp(treeNode* node);
void Args(treeNode* root, Type* type);
Type* Specifier(treeNode* node,int depth);
Type* StructSpecifier(treeNode* node,int depth);
void FunDec(treeNode* node, Type* ret, int defined);
void VarList(treeNode* node, Type* headType);
void ParamDec(treeNode* node, Type* headType);
void ExtDecList(treeNode* node, Type* type);
Type* VarDec(treeNode* node, Type *type, Type* headType,int defined,FieldList* para,int depth);
void DefList(treeNode* node, Type* headType,int defined,int depth);
void Def(treeNode* node,Type * headType, int defined,int depth);
void DecList(treeNode *node, Type* type,Type * headType,  int defined,int depth);
void Dec(treeNode *node, Type* type, Type* headType, int defined,int depth) ;
int BeSameType(Type* Ltype,Type*Rtype);
int checkDeclaration(Type* Ltype,Type*Rtype);
int checkOperator(Type* Ltype,Type*Rtype, char* operator);
void serror(int errorType,int line,char* msg);
int getTypeSize(Type* typeElement);

#endif