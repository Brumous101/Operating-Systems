#include "utility.h"
#include "kprintf.h"

extern void* interrupt_table_begin;
extern void* interrupt_table_end;
void interrupt_init(){
    kmemcpy((void*)0, &interrupt_table_begin,
        4*(&interrupt_table_end-&interrupt_table_begin));
}
void handler_reset()
{
    kprintf("RESET");
    halt();
}

void handler_undefined()
{
    kprintf("UNDEFINED");
    halt();
}

void handler_svc()
{
    kprintf("SVC");
    halt();
}

void handler_prefetchabort()
{
    kprintf("PREFETCH ABORT");
    halt();
}

void handler_dataabort()
{
    kprintf("DATA ABORT");
    halt();
}

void handler_reserved()
{
    kprintf("RESERVED");
    halt();
}

void handler_irq()
{
    kprintf("IRQ");
    halt();
}

void handler_fiq()
{
    kprintf("FIQ");
    halt();
}
