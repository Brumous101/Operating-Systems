#include "syscalldefs.h"


int make_syscall(unsigned req, unsigned p1, unsigned p2, unsigned p3){
    register unsigned tmp0 asm ("r0");
    register unsigned tmp1 asm ("r1");
    register unsigned tmp2 asm ("r2");
    register unsigned tmp3 asm ("r3");
    tmp0=req;
    tmp1=p1;
    tmp2=p2;
    tmp3=p3;
    asm volatile("svc #0" 
        : "+r"(tmp0) 
        : "r"(tmp1), "r"(tmp2), "r"(tmp3) 
        : "memory","cc");
    return tmp0;
}

int main(int argc, char* argv[])
{
    char buf[32];
    
    int fd = make_syscall(SYSCALL_OPEN, (unsigned) "article4.txt", 0, 0);
    if( fd >= 0 ){
        while(1){
            int rv = make_syscall( SYSCALL_READ, fd, (unsigned) buf, sizeof(buf));
            if( rv <= 0 )
                break;
            make_syscall(SYSCALL_WRITE, 1, (unsigned)buf, rv);
        }
    } else {
        make_syscall(SYSCALL_WRITE, 1, (unsigned)"Cannot open article4.txt", 24 );
    }
    
    while(1){
    }
    
    return 0;
    
}
