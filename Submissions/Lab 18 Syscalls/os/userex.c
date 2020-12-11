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
    asm volatile(
          "svc #0" 
        : "+r"(tmp0) 
        : "r"(tmp1), "r"(tmp2), "r"(tmp3) 
        : "memory","cc");
    return tmp0;
}

int main(int argc, char* argv[])
{
    int v = make_syscall(SYSCALL_TEST,1,2,3);
    make_syscall(SYSCALL_TEST,10+v,20+v,30+v);
    
    while(1){
    }
    
    return 0;
}
