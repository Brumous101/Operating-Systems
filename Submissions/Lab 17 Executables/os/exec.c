#include "exec.h"
#include "disk.h"
#include "kprintf.h"

int exec( const char* filename ){
    char* p = (char*) 0x400000;
    int fd = file_open(filename,0);
    int nr;
    if(fd < 0){
        kprintf("fd returned %d. filename is not valid.\n",fd);
        return -1;
    }
    while(1){
        //kprintf("exec is called!\n");
        nr = file_read(fd, p, 1000000);
        if(nr < 0){
            kprintf("nr returned a value of: %d\n",nr);
            //not in jim's code I added the break
            file_close(fd);
            return nr;
        }
        if(nr == 0){
            //kprintf("nr returned a value of: %d\n",nr);
            break;
        }
    p += nr;
    }
    file_close(fd);

    //the moving registers around
    asm volatile(
    "mrs r0,cpsr\n"
    "and r0,#0xffffffe0\n"
    "orr r0,#0x10\n"
    "msr cpsr,r0\n"
    "mov sp,#0x800000\n"
    "mov pc,#0x400000\n"
    : : : "memory"
    ); 
    kprintf("\nend of exec! \n");
    return 1;
}