#include"semantic.h"

hashNode* hashTable[HASHSIZE+1];

void tableInit()
{
    for(int i=0;i<=HASHSIZE;++i)
        hashTable[i]=NULL;
}

unsigned hash_pjw(char* name)
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
        printf("name:%s\n",hashTable[i]->name);
}
}

void insert(hashNode* node)
{
    unsigned index = hash_pjw(node->name);
    node->next=hashTable[index];
    hashTable[index]=node;
}

hashNode* search(char * name)
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

hashNode* newSymbol(char* name, Type* type, int line, int defined)
{
    hashNode* temp = (hashNode*)malloc(sizeof(hashNode*));
    temp->name = name;
    temp->type = type;
    temp->line = line;
    temp->defined = defined;
    return temp;
}

void addSymbol(char* name, Type* type, int line, int defined)
{
    if(!type)
        return;    
    insert(newSymbol(name, type, line, defined));
}

void Program(treeNode* node)
{
    if (node==NULL)
        return;
    
    if (node->child!=NULL) 
    {
        ExtDefList(node->child);
    }
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
    // Specifier FunDec CompSt
     else if(!strcmp("FunDec",node->child->bro->name))
     {
        /*
        Type* type = Specifier(node->child);
        FunDec(node->child->bro);
        CompSt(node->child->bro->bro);
        */
     }
    // Specifier SEMI
    else
    {

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
    /*
    //Specifier -> StructSpecifier
    else    
        return StructSpecifier(node->child);
        */
}

/*
void FunDec(treeNode* node, Type* ret,int defined)
FieldList* VarList(treeNode* node, Type* headType)
FieldList ParamDec(treeNode* node, Type headType)
void CompSt(treeNode* node, Type ret, bool defined) 
*/

/*
Type StructSpecifier(treeNode* node)
FieldList DefList_Structure(treeNode* node, Type headType)
FieldList Def_Structure(treeNode* node, Type headType)
FieldList DecList_Structure(treeNode* node, Type type, Type headType)
FieldList Dec_Structure(treeNode* node, Type type, Type headType)
FieldList VarDec_Structure(treeNode* node, Type type, Type headType)
*/
/*
Type FindStructureFiled(Type type, char * name, int line)
bool isRightValuesOnly(treeNode* node)
*/

void ExtDecList(treeNode* node, Type* type)
{
    if (node==NULL)
        return;
    //ExtDecList -> VarDec COMMA ExtDecList
    if(node->child->bro!=NULL) 
    {
        VarDec(node->child, type, 1); 
        ExtDecList(node->child->bro->bro,type);
    }
    //ExtDecList -> VarDec
    else 
    { 
        VarDec(node->child, type, 1);
    }
}

void VarDec(treeNode* node, Type *type, int defined)
{
    if (node==NULL)
        return;
    //VarDec -> ID
    if(!strcmp("ID",node->child->name))
        addSymbol(node->child->s_val, type, node->child->line, defined);
    //VarDec -> VarDec LP INT RP
    else  
    {	
        Type* temp = (Type*)malloc(sizeof(Type));
        temp->kind = ARRAY;
        temp->u.array.elem = type;   //???
        temp->u.array.size = node->child->bro->bro->i_val;
        VarDec(node->child, temp, defined);
    }
}

void DefList(treeNode* node, int defined)
{
    if (node==NULL)
        return;
     // DefList-> Def DefList    
    if(node->child)
    {
        Def(node->child, defined);
        DefList(node->child->bro, defined);
    }
    // DefList-> empty
    else
    {
        
    }
}

void Def(treeNode* node, int defined)
{
    if (node==NULL)
        return;
    //Def -> Specifier DecList SEMI
    if(node->child)
    {
        Type* type = Specifier(node->child);
        if(type == NULL) 
            return;
        DecList(node->child->bro, type, defined);
    }
}

void DecList(treeNode *node, Type* type,  int defined)
 {
    if (node==NULL)
        return;
    //DecList -> Dec
    if(node->child->bro == NULL) 
    {
        Dec(node->child, type, defined);
    }
    //DecList -> Dec COMMA DecList
    else   
    {
        Dec(node->child, type, defined);
        DecList(node->child->bro->bro, type, defined);
    }
}

void Dec(treeNode *node, Type* type,  int defined) 
{
    if (node==NULL)
        return;
    //DecList -> Dec
    if(node->child->bro == NULL)
    {
        VarDec(node->child, type, defined);
    }
    /*
    //DecList -> Dec COMMA DecList
    else   
    {
        VarDec(node->child, type, defined);
        Exp(node->child->bro->bro);
    }
    */
}

