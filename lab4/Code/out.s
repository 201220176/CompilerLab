.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

fact:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -40
  lw $t0, 8($fp)
  sw $t0, -4($fp)
  lw $t0, -4($fp)
  sw $t0, -8($fp)
  li $t0, 1
  sw $t0, -12($fp)
  lw $t1, -8($fp)
  lw $t2, -12($fp)
  beq $t1, $t2, label1
  j label2
label1:
  lw $t0, -4($fp)
  sw $t0, -16($fp)
  lw $v0, -16($fp)
  addi $sp, $fp, 4
  lw $fp, 0($fp)
  jr $ra
  j label3
label2:
  lw $t0, -4($fp)
  sw $t0, -20($fp)
  lw $t0, -4($fp)
  sw $t0, -24($fp)
  li $t0, 1
  sw $t0, -28($fp)
  lw $t1, -24($fp)
  lw $t2, -28($fp)
  sub $t0, $t1, $t2
  sw $t0, -32($fp)
  addi $sp, $sp, -4
  lw $t0, -32($fp)
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, -36($fp)
  addi $sp, $sp, 4
  lw $t1, -20($fp)
  lw $t2, -36($fp)
  mul $t0, $t1, $t2
  sw $t0, -40($fp)
  lw $v0, -40($fp)
  addi $sp, $fp, 4
  lw $fp, 0($fp)
  jr $ra
label3:

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -88
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, -12($fp)
  lw $t0, -12($fp)
  sw $t0, -16($fp)
  lw $t0, -16($fp)
  sw $t0, -20($fp)
  li $t0, 1
  sw $t0, -24($fp)
  lw $t1, -20($fp)
  lw $t2, -24($fp)
  bgt $t1, $t2, label4
  j label5
label4:
  lw $t0, -16($fp)
  sw $t0, -28($fp)
  addi $sp, $sp, -4
  lw $t0, -28($fp)
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, -32($fp)
  addi $sp, $sp, 4
  lw $t0, -32($fp)
  sw $t0, -36($fp)
  j label6
label5:
  li $t0, 1
  sw $t0, -40($fp)
  lw $t0, -40($fp)
  sw $t0, -36($fp)
label6:
  addi $t0, $fp, -8
  sw $t0, -44($fp)
  li $t0, 1
  sw $t0, -48($fp)
  lw $t1, -48($fp)
  li $t2, 4
  mul $t0, $t1, $t2
  sw $t0, -52($fp)
  lw $t1, -44($fp)
  lw $t2, -52($fp)
  add $t0, $t1, $t2
  sw $t0, -56($fp)
  lw $t0, -36($fp)
  sw $t0, -60($fp)
  lw $t0, -56($fp)
  lw $t1, -60($fp)
  sw $t1, 0($t0)
  lw $t0, -36($fp)
  sw $t0, -64($fp)
  lw $a0, -64($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  addi $t0, $fp, -8
  sw $t0, -68($fp)
  li $t0, 1
  sw $t0, -72($fp)
  lw $t1, -72($fp)
  li $t2, 4
  mul $t0, $t1, $t2
  sw $t0, -76($fp)
  lw $t1, -68($fp)
  lw $t2, -76($fp)
  add $t0, $t1, $t2
  sw $t0, -80($fp)
  lw $t1, -80($fp)
  lw $t0, 0($t1)
  sw $t0, -84($fp)
  lw $a0, -84($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $t0, 0
  sw $t0, -88($fp)
  lw $v0, -88($fp)
  addi $sp, $fp, 4
  lw $fp, 0($fp)
  jr $ra
