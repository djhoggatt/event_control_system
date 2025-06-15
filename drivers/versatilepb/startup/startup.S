.globl VTABLE

.section ".startup","ax"
.arm
.globl Reset_Handler
.globl _start
_start:
Reset_Handler:
    ldr sp, =topOfStack  @ Initialize the stack
    bl main              @ Should not return
    bl Reset_Handler