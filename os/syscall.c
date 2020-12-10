#include "errno.h"
#include "syscalldefs.h"
#include "utility.h"


int syscall_handler( int req, int p1, int p2, int p3 ){
    switch(req){
        case SYSCALL_TEST:
            kprintf("Syscall test! %d %d %d\n",p1,p2,p3);
            return p1+p2+p3;
        case SYSCALL_OPEN:
            ;
        case SYSCALL_CLOSE:
            ;
        case SYSCALL_READ:
            ;
        case SYSCALL_WRITE:
            ;
    }
    return -ENOSYS; //no such system call
}

void handler_svc()
{
    kprintf("SVC");
    halt();
}