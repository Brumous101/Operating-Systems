.section .text
.global start
.global interrupt_table_begin
.global interrupt_table_end

start:
    ldr sp, =0xfff0
    b kmain

.ltorg
interrupt_table_begin:
    ldr pc, =asm_handler_reset
    ldr pc, =asm_handler_undefined
    ldr pc, =asm_handler_svc
    ldr pc, =asm_handler_prefetchabort
    ldr pc, =asm_handler_dataabort
    ldr pc, =asm_handler_reserved
    ldr pc, =asm_handler_irq
    ldr pc, =asm_handler_fiq
.ltorg
interrupt_table_end:
asm_handler_reset:
    ldr sp, =reset_stack
    sub lr,#4
    push {r0-r12,lr}
    bl handler_reset
    pop {r0-r12,lr}
    subs pc,lr,#0
asm_handler_undefined:
    ldr sp, =undefined_stack
    sub lr,#4
    push {r0-r12,lr}
    bl handler_undefined
    pop {r0-r12,lr}
    subs pc,lr,#0
asm_handler_svc:
    ldr sp, =svc_stack
    push {r1-r12,lr}
    bl syscall_handler
    pop {r1-r12,lr}
    subs pc,lr,#0
asm_handler_prefetchabort:
    ldr sp, =prefetchabort_stack
    sub lr,#4
    push {r0-r12,lr}
    bl handler_prefetchabort
    pop {r0-r12,lr}
    subs pc,lr,#0
asm_handler_dataabort:
    ldr sp, =dataabort_stack
    sub lr,#8
    push {r0-r12,lr}
    bl handler_dataabort
    pop {r0-r12,lr}
    subs pc,lr,#0
asm_handler_reserved:
    ldr sp, =reserved_stack
    sub lr,#4
    push {r0-r12,lr}
    bl handler_reserved
    pop {r0-r12,lr}
    subs pc,lr,#0
asm_handler_irq:
    ldr sp, =irq_stack
    sub lr,#4
    push {r0-r12,lr}
    bl handler_irq
    pop {r0-r12,lr}
    subs pc,lr,#0
asm_handler_fiq:
    ldr sp, =fiq_stack
    sub lr,#4
    push {r0-r12,lr}
    bl handler_fiq
    pop {r0-r12,lr}
    subs pc,lr,#0


.section .data

.rept 1024
.word 0
.endr
reset_stack:

.rept 1024
.word 0
.endr
undefined_stack:

.rept 1024
.word 0
.endr
svc_stack:

.rept 1024
.word 0
.endr
prefetchabort_stack:

.rept 1024
.word 0
.endr
dataabort_stack:

.rept 1024
.word 0
.endr
reserved_stack:

.rept 1024
.word 0
.endr
irq_stack:

.rept 1024
.word 0
.endr
fiq_stack: