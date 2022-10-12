#include"tree.h"
#include <stdio.h>
#include <string.h>

#define HASHSIZE    0x3fff    



typedef struct Type{
    enum { BASIC, ARRAY, STRUCTURE,FUNCTION} kind;
    union {
        //基本类型
        int  basic;
        //数组类型信息包括元素类型与数组大小构成
        struct {struct Type* elem; int size; } array;
        //结构体类型信息是一个链表
        struct FieldList* structure;
    } u;
}Type;

typedef struct FieldList {
    char* name; //域的名字
    Type* type; //域的类型
    struct FieldList* tail; //下一个域
}FieldList;

typedef struct hashNode{
    char* name;
    Type* type;
    int line;
    int defined;
    int size;
    struct hashNode* next;
}hashNode;


void tableInit();
void printTable();
unsigned hash_pjw(char* name);
void insert(hashNode* node);
hashNode* search(char * name);
hashNode* newSymbol(char* name, Type* type, int line, int defined);
void addSymbol(char* name, Type* type, int line, int defined);
void Program(treeNode* node);
void ExtDefList(treeNode* node);
void ExtDef(treeNode* node);
Type* Specifier(treeNode* node);
void ExtDecList(treeNode* node, Type* type);
void VarDec(treeNode* node, Type *type, int defined);
void DefList(treeNode* node, int defined);
void Def(treeNode* node, int defined);
void DecList(treeNode *node, Type* type,  int defined);
void Dec(treeNode *node, Type* type,  int defined) ;
