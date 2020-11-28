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
    ldr pc, =asm_handler_reset
asm_handler_undefined:
    ldr pc, =asm_handler_undefined
asm_handler_svc:
    ldr pc, =asm_handler_svc
asm_handler_prefetchabort:
    ldr pc, =asm_handler_prefetchabort
asm_handler_dataabort:
    ldr pc, =asm_handler_dataabort
asm_handler_reserved:
    ldr pc, =asm_handler_reserved
asm_handler_irq:
    ldr pc, =asm_handler_irq
asm_handler_fiq:
    ldr pc, =asm_handler_fiq