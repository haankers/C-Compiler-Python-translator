.globl func
.ent func
.type func, @function
func:
addiu $sp , $sp , -24
sw $31 , 20($sp)
sw $fp , 16($sp)
addu $fp , $sp , $0
addiu $3 , $0 , 0x1
addiu $3 , $0 , 0xFFFF
sll $3 , $3 , 0xA
mult $3 , $3
mflo $2
sw $2 , 0($fp)
addiu $2 , $0 , 0x0
sw $2 , 4($fp)
addiu $2 , $0 , 0x1
sw $2 , 8($fp)
addiu $2 , $0 , 0x2
sw $2 , 12($fp)
lw $2 , 0($fp)
lw $3 , 0($fp)
slt $2 , $3 , $2
xori $2 , $2 , 0x1
beq $2 , $0 , $L1
nop
lw $2 , 0($fp)
lw $3 , 4($fp)
slt $2 , $3 , $2
xori $2 , $2 , 0x1
beq $2 , $0 , $L1
nop
lw $2 , 0($fp)
lw $3 , 8($fp)
slt $2 , $3 , $2
xori $2 , $2 , 0x1
beq $2 , $0 , $L1
nop
lw $2 , 8($fp)
lw $3 , 12($fp)
slt $2 , $3 , $2
xori $2 , $2 , 0x1
beq $2 , $0 , $L1
nop
lw $2 , 4($fp)
lw $3 , 4($fp)
slt $2 , $3 , $2
xori $2 , $2 , 0x1
beq $2 , $0 , $L1
nop
lw $2 , 8($fp)
lw $3 , 0($fp)
slt $2 , $3 , $2
xori $2 , $2 , 0x1
beq $2 , $0 , $L1
nop
addiu $2 , $0 , 0x1
addu $2 , $2 , $0
andi $2 , $2 , 0x00ff
b $E0
nop
b $L2
nop
$L1:
addiu $2 , $0 , 0xa
addu $2 , $2 , $0
andi $2 , $2 , 0x00ff
b $E0
nop
$L2:
$L3:
$L4:
$L5:
$L6:
$L7:
addiu $2 , $0 , 0x14
addu $2 , $2 , $0
andi $2 , $2 , 0x00ff
b $E0
nop
$E0:
addu $sp , $fp , $0
lw $fp , 16($sp)
sw $31 , 20($sp)
addiu $sp , $sp , 24
j $31
nop
.end func
