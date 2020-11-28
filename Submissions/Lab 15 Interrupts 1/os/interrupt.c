#include "utility.h"

extern void* interrupt_table_begin;
extern void* interrupt_table_end;
void interrupt_init(){
    kmemcpy((void*)0, &interrupt_table_begin,
        4*(&interrupt_table_end-&interrupt_table_begin));
}