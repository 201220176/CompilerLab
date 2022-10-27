#include"semantic.h"

void translate_init();
void translate(treeNode *root);
void translate_Program(treeNode* node);
void translate_ExtDefList(treeNode* node);
void translate_ExtDef(treeNode* node);
void translate_CompSt(treeNode* node) ;
void translate_StmtList(treeNode* node);
void translate_Stmt(treeNode* node);
Type* translate_Exp(treeNode* node);
void translate_Args(treeNode* root);
void translate_FunDec(treeNode* node);
void translate_VarList(treeNode* node);
void translate_ParamDec(treeNode* node);
void translate_ExtDecList(treeNode* node);
Type* translate_VarDec(treeNode* node);
void translate_DefList(treeNode* node);
void translate_Def(treeNode* node);
void translate_DecList(treeNode *node);
void translate_Dec(treeNode *node) ;