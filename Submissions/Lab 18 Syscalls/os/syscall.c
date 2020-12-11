#include "errno.h"
#include "syscalldefs.h"
#include "kprintf.h"
#include "disk.h"

int syscall_handler( int req, int p1, int p2, int p3 ){
    switch(req){
        case SYSCALL_TEST:
            //kprintf("Syscall test! %d %d %d\n",p1,p2,p3);
            return p1+p2+p3;
        case SYSCALL_OPEN:
            //p1 = filename
            //p2 = flags
            //kprintf("SYSCALL OPEN WAS CALLED p1 = %d p2 = %d p3 = %d\n",p1,p2,p3);
            if(p1 == 0 || p1 < 0)
                return -ENOSYS;
            else if(p1 == 1 || p1 == 2)
                return -EINVAL;
            else
                return file_open( (char*) p1, p2 );
        case SYSCALL_CLOSE:
            if(p1 == 0 || p1 < 0)
                return -ENOSYS;
            else if(p1 == 1 || p1 == 2)
                return -EINVAL;
            else
                return file_close( p1 );
        case SYSCALL_READ:
            // p1 = fd, p2=buffer, p3=count
            //kprintf("SYSCALL READ WAS CALLED p1 = %d p2 = %d p3 = %d\n",p1,p2,p3);
            if( p2 < 0x400000 )
                return -EFAULT;
            if(p1 == 0 || p1 < 0)
                return -ENOSYS;
            if(p1 == 1 || p1 == 2)
                return -EINVAL;
            else
                return file_read( p1, (void*) p2, p3 );
        case SYSCALL_WRITE://console.putc or serial outchar
            //kprintf("SYSCALL Write WAS CALLED p1 = %d p2 = %d p3 = %d\n",p1,p2,p3);
            if(p1 == 0 || p1 < 0)
                return -EINVAL;
            if(p1 >= 3)
                return file_write( p1, (void*) p2, p3 );
            if(p2 < p3){
                for(int j = 0; j < p2; ++j)
                    kprintf("%c", ((char*) p2)[j]);
                return 1;
            }
            else{
                for(int j = 0; j < p3; ++j)
                    kprintf("%c", ((char*) p2)[j]);
                return 1;
            }
    }
    return -ENOSYS; //no such system call
}