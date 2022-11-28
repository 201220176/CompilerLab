#include"ObjCode.h"
#include"IR.h"
extern InterCodes* head;
extern InterCodes* tail;
extern int varCount , tempCount , labelCount;
int* varOffset;

int calcuOffset(Operand* op, int fpoffset)
{
    //若第一次出现该变量，则将其相对于fp的偏移设置为该处
    if (op->kind == VARIABLE_O || op->kind == TEMP_O || op->kind == PARAMETER_O) {
        int offset = 0;
        if (op->kind == TEMP_O) {
            offset = varCount - 1;
        }
        if (varOffset[op->u.var_no + offset] == -1) {
            varOffset[op->u.var_no + offset] = fpoffset;
            return 4;
        } else
            return 0;
    } else
        return 0;
}

//为本函数每个出现的变量，设定其相对与fp存储的偏移量，于varOffset中
int allocVar(InterCodes* begin)
{
    int fpoffset = 4;
    InterCodes* p = begin;
    while (p && p->code.kind != FUNCTION_I) {
            switch (p->code.kind) {
            case ASSIGN_I: {
                fpoffset += calcuOffset(p->code.u.assign.left, fpoffset);
                fpoffset += calcuOffset(p->code.u.assign.right, fpoffset);
            } break;
            case ADD_I:
            case SUB_I:
            case MUL_I:
            case DIV_I: {
                fpoffset += calcuOffset(p->code.u.binop.res, fpoffset);
                fpoffset += calcuOffset(p->code.u.binop.op1, fpoffset);
                fpoffset += calcuOffset(p->code.u.binop.op2, fpoffset);
            } break;
            case IF_I: {
                fpoffset += calcuOffset(p->code.u.cond.op1, fpoffset);
                fpoffset += calcuOffset(p->code.u.cond.op2, fpoffset);
            } break;
            case DEC_I:
                if (varOffset[p->code.u.dec.op->u.var_no] == -1) {
                    varOffset[p->code.u.dec.op->u.var_no] = fpoffset + p->code.u.dec.size - 4;
                    fpoffset += p->code.u.dec.size;
                }
                break;
            case CALL_I: {
                fpoffset += calcuOffset(p->code.u.unaryop.res, fpoffset);
            } break;
            case RETURN_I:
            case ARG_I:
            case PARAM_I:
            case READ_I:
            case WRITE_I:
                fpoffset += calcuOffset(p->code.u.single.op, fpoffset);
                break;
            }
        p = p->next;
    }
    return fpoffset - 4;
}

void ObjCodeGen(char* outputfile)
{
   //记录每个变量在fp中的偏移量
    int n = varCount + tempCount - 1;
    varOffset = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        varOffset[i] = -1;
    }

    InterCodes* p = head->next;
    FILE* f = fopen(outputfile, "w");
    //初始化，写入read和write函数
    fprintf(f,".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
    fprintf(f,"read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n\n");
    fprintf(f,"write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
    //逐行处理中间代码
    while (p)
    {
        switch (p->code.kind)
        {
          case FUNCTION_I:
                fprintf(f, "\n%s:\n", toOp(p->code.u.single.op));
                //sp下移以将当前fp压栈，将fp置为当前活动记录开始处
                fprintf(f, "  addi $sp, $sp, -4\n");
                fprintf(f, "  sw $fp, 0($sp)\n");
                fprintf(f, "  move $fp, $sp\n");
                //为本函数用到的变量分配空间
                fprintf(f, "  addi $sp, $sp, -%d\n", allocVar(p->next));
                break;
        case LABEL_I:
                fprintf(f, "%s:\n", toOp(p->code.u.single.op));
                break;
        case GOTO_I:
                fprintf(f, "  j %s\n", toOp(p->code.u.single.op));
                break;
        case ASSIGN_I: {
                Operand* l = p->code.u.assign.left;
                Operand* r = p->code.u.assign.right;
                int loffset = 0;
                if (l->kind == TEMP_O)
                    loffset = varCount - 1;
                switch (p->code.u.assign.type) {
                case NORMAL_I: {
                    //目标寄存器的位置
                    if (r->kind == CONSTANT_O) {
                        //将立即数右值载入t0
                        fprintf(f, "  li $t0, %d\n", r->u.value);
                        fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[l->u.var_no + loffset]);
                    } 
                    else {
                        //找到右值的栈中位置，载入t0
                        int roffset = 0;
                        if (r->kind == TEMP_O)
                            roffset = varCount - 1;
                        fprintf(f, "  lw $t0, -%d($fp)\n", varOffset[r->u.var_no + roffset]);
                        fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[l->u.var_no + loffset]);
                    }
                } break;
                case GETADDR_I: {
                    //目标寄存器的位置
                    int roffset = 0;
                    //不会出现取临时变量的地址
                    if (r->kind == TEMP_O) {
                        exit(-1);
                        roffset = varCount - 1;
                    }
                    //将右边地址载入t0
                    fprintf(f, "  addi $t0, $fp, -%d\n", varOffset[r->u.var_no + roffset]);
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[l->u.var_no + loffset]);
                } break;
                case GETVAL_I: {
                    //目标寄存器的位置
                    int roffset = 0;
                    if (r->kind == TEMP_O)
                        roffset = varCount - 1;
                    //将右边(地址)载入t0
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[r->u.var_no + roffset]);
                    //将右边(值)载入t0
                    fprintf(f, "  lw $t0, 0($t1)\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[l->u.var_no + loffset]);
                } break;
                case SETVAL_I: {
                    //将左边(地址)载入t0
                    fprintf(f, "  lw $t0, -%d($fp)\n", varOffset[l->u.var_no + loffset]);
                    int roffset = 0;
                    if (r->kind == TEMP_O)
                        roffset = varCount - 1;
                    //将右边载入t1
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[r->u.var_no + roffset]);
                    //将t1存入左边的地址
                    fprintf(f, "  sw $t1, 0($t0)\n");
                } break;
                default:
                    break;
                }
            } 
            break;
        case ADD_I: {
                Operand* res = p->code.u.binop.res;
                Operand* op1 = p->code.u.binop.op1;
                Operand* op2 = p->code.u.binop.op2;
                int resoffset = 0, op1offset = 0, op2offset = 0;
                if (res->kind == TEMP_O)
                    resoffset = varCount - 1;
                if (op1->kind == TEMP_O)
                    op1offset = varCount - 1;
                if (op2->kind == TEMP_O)
                    op2offset = varCount - 1;

                if (op1->kind != CONSTANT_O && op2->kind == CONSTANT_O) {
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  addi $t0, $t1, %d\n", op2->u.value);
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else if (op1->kind == CONSTANT_O && op2->kind != CONSTANT_O) {
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  addi $t0, $t2, %d\n", op1->u.value);
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else  if(op1->kind != CONSTANT_O && op2->kind != CONSTANT_O)
                {
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  add $t0, $t1, $t2\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
                else
                {
                    fprintf(f,"  li $t0, %d\n",op1->u.value+op2->u.value);
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
            } 
            break;
        case SUB_I: {
                Operand* res = p->code.u.binop.res;
                Operand* op1 = p->code.u.binop.op1;
                Operand* op2 = p->code.u.binop.op2;
                int resoffset = 0, op1offset = 0, op2offset = 0;
                if (res->kind == TEMP_O)
                    resoffset = varCount - 1;
                if (op1->kind == TEMP_O)
                    op1offset = varCount - 1;
                if (op2->kind == TEMP_O)
                    op2offset = varCount - 1;
                if (op1->kind != CONSTANT_O && op2->kind == CONSTANT_O) {
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  addi $t0, $t1, %d\n", -op2->u.value);
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else if (op1->kind == CONSTANT_O && op2->kind != CONSTANT_O) {
                    fprintf(f, "  li $t1, %d\n", op1->u.value);
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  sub $t0, $t1, $t2\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else if(op1->kind != CONSTANT_O && op2->kind != CONSTANT_O){
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  sub $t0, $t1, $t2\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
                else
                {
                    fprintf(f,"  li $t0, %d\n",(op1->u.value-op2->u.value));
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
            } 
            break;
            case MUL_I: {
                Operand* res = p->code.u.binop.res;
                Operand* op1 = p->code.u.binop.op1;
                Operand* op2 = p->code.u.binop.op2;
                int resoffset = 0, op1offset = 0, op2offset = 0;
                if (res->kind == TEMP_O)
                    resoffset = varCount - 1;
                if (op1->kind == TEMP_O)
                    op1offset = varCount - 1;
                if (op2->kind == TEMP_O)
                    op2offset = varCount - 1;
                if (op1->kind != CONSTANT_O && op2->kind == CONSTANT_O) {
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  li $t2, %d\n", op2->u.value);
                    fprintf(f, "  mul $t0, $t1, $t2\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else if (op1->kind == CONSTANT_O && op2->kind != CONSTANT_O) {
                    fprintf(f, "  li $t1, %d\n", op1->u.value);
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  mul $t0, $t1, $t2\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else if(op1->kind != CONSTANT_O && op2->kind != CONSTANT_O){
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  mul $t0, $t1, $t2\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
                else
                {
                    fprintf(f,"  li $t0, %d\n",(op1->u.value*op2->u.value));
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
            } break;
        case DIV_I: {
                Operand* res = p->code.u.binop.res;
                Operand* op1 = p->code.u.binop.op1;
                Operand* op2 = p->code.u.binop.op2;
                int resoffset = 0, op1offset = 0, op2offset = 0;
                if (res->kind == TEMP_O)
                    resoffset = varCount - 1;
                if (op1->kind == TEMP_O)
                    op1offset = varCount - 1;
                if (op2->kind == TEMP_O)
                    op2offset = varCount - 1;
                if (op1->kind != CONSTANT_O && op2->kind == CONSTANT_O) {
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  li $t2, %d\n", op2->u.value);
                    fprintf(f, "  div $t1, $t2\n");
                    fprintf(f, "  mflo $t0\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else if (op1->kind == CONSTANT_O && op2->kind != CONSTANT_O) {
                    fprintf(f, "  li $t1, %d\n", op1->u.value);
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  div $t1, $t2\n");
                    fprintf(f, "  mflo $t0\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                } else if (op1->kind != CONSTANT_O && op2->kind != CONSTANT_O){
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                    fprintf(f, "  div $t1, $t2\n");
                    fprintf(f, "  mflo $t0\n");
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
                else
                {
                    fprintf(f,"  li $t0, %d\n",(op1->u.value/op2->u.value));
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                }
            } 
            break;
        case IF_I: {
                Operand* op1 = p->code.u.cond.op1;
                Operand* op2 = p->code.u.cond.op2;
                Operand* target = p->code.u.cond.target;
                int resoffset = 0, op1offset = 0, op2offset = 0;
                if (op1->kind == TEMP_O)
                    op1offset = varCount - 1;
                if (op2->kind == TEMP_O)
                    op2offset = varCount - 1;
                if (op1->kind == CONSTANT_O)
                    fprintf(f, "  li $t1, %d\n", op1->u.value);
                else
                    fprintf(f, "  lw $t1, -%d($fp)\n", varOffset[op1->u.var_no + op1offset]);
                if (op2->kind == CONSTANT_O)
                    fprintf(f, "  li $t2, %d\n", op2->u.value);
                else
                    fprintf(f, "  lw $t2, -%d($fp)\n", varOffset[op2->u.var_no + op2offset]);
                char* relop = p->code.u.cond.relop;
                if (!strcmp("==", relop))
                    fprintf(f, "  beq $t1, $t2, %s\n", toOp(target));
                else if (!strcmp("!=", relop))
                    fprintf(f, "  bne $t1, $t2, %s\n", toOp(target));
                else if (!strcmp(">", relop))
                    fprintf(f, "  bgt $t1, $t2, %s\n", toOp(target));
                else if (!strcmp("<", relop))
                    fprintf(f, "  blt $t1, $t2, %s\n", toOp(target));
                else if (!strcmp(">=", relop))
                    fprintf(f, "  bge $t1, $t2, %s\n", toOp(target));
                else if (!strcmp("<=", relop))
                    fprintf(f, "  ble $t1, $t2, %s\n", toOp(target));
            } 
            break;
            case RETURN_I: {     
                Operand* ret = p->code.u.single.op;
                int retoffset = 0;
                //返回值存入v0
                if (ret->kind == TEMP_O)
                    retoffset = varCount - 1;
                if (ret->kind == CONSTANT_O) {
                    fprintf(f, "  li $v0, %d\n", ret->u.value);
                } else {
                    fprintf(f, "  lw $v0, -%d($fp)\n", varOffset[ret->u.var_no + retoffset]);
                }
                //恢复sp和fp
                fprintf(f, "  addi $sp, $fp, 4\n  lw $fp, 0($fp)\n");
                //跳转至返回地址
                fprintf(f, "  jr $ra\n");
            } break;
            case ARG_I: {
                //栈顶指针下移
                fprintf(f, "  addi $sp, $sp, -4\n");
                Operand* op = p->code.u.single.op;
                int opoffset = 0;
                //将参数加载到t0
                if (op->kind == TEMP_O)
                    opoffset = varCount - 1;
                if (op->kind == CONSTANT_O) {
                    fprintf(f, "  li $t0, %d\n", op->u.value);
                }
                else {
                    fprintf(f, "  lw $t0, -%d($fp)\n", varOffset[op->u.var_no + opoffset]);
                }
                //将参数存至当前栈顶指针处
                fprintf(f, "  sw $t0, 0($sp)\n");
            } break;
            case CALL_I: {
                Operand* res = p->code.u.unaryop.res;
                Operand* op = p->code.u.unaryop.op;
                //将当前返回地址压栈
                fprintf(f, "  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n");
                //跳转
                fprintf(f, "  jal %s\n", toOp(op));
                //跳转回来后，将返回地址退栈恢复
                fprintf(f, "  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n");
                int resoffset = 0;
                if (res->kind == TEMP_O)
                    resoffset = varCount - 1;
                //将返回值存入res中
                fprintf(f, "  sw $v0, -%d($fp)\n", varOffset[res->u.var_no + resoffset]);
                hashNode* func = search(op->u.func_name);
                //将传入的实参退栈
                fprintf(f, "  addi $sp, $sp, %d\n",func->type->u.function.argCount * 4);
            } break;
            case PARAM_I: {
                //在此处将FUNCTION后面的所有PARA一起处理
                hashNode* func = search(p->prev->code.u.single.op->u.func_name);
                int argcount = func->type->u.function.argCount;
                for (int i = 0; i < argcount; ++i) {
                    //将实参 i 载入到t0
                    fprintf(f, "  lw $t0, %d($fp)\n", 8 + i * 4);
                    //将参数给传递给，函数内局部变量存储的位置
                    fprintf(f, "  sw $t0, -%d($fp)\n", varOffset[p->code.u.single.op->u.var_no]);
                    if(i==argcount-1)
                        break;
                    p = p->next;
                }
            } 
            break;
            case READ_I: {
                Operand* op = p->code.u.single.op;
                int opoffset = 0;
                if (op->kind == TEMP_O) {
                    opoffset = varCount - 1;
                }
                fprintf(f, "  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n  jal read\n  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n");
                fprintf(f, "  sw $v0, -%d($fp)\n", varOffset[op->u.var_no + opoffset]);
            } break;
            case WRITE_I: {
                Operand* op = p->code.u.single.op;
                //将参数传递给a0
                if (op->kind == CONSTANT_O) {
                    fprintf(f, "  li $a0, %d\n", op->u.value);
                } else {
                    int opoffset = 0;
                    if (op->kind == TEMP_O) {
                        opoffset = varCount - 1;
                    }
                    fprintf(f, "  lw $a0, -%d($fp)\n", varOffset[op->u.var_no + opoffset]);
                }
                fprintf(f, "  addi $sp, $sp, -4\n  sw $ra, 0($sp)\n  jal write\n  lw $ra, 0($sp)\n  addi $sp, $sp, 4\n");
            } break;
        default:
            break;
        }
        p = p->next;
    }
    fclose(f);
    
}