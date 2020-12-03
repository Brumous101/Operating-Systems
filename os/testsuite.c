#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#include "kprintf.h"

extern int file_open(const char* fname, int flags);
extern int file_read(int fd, void* buf, int num);
extern int file_seek(int fd, int offset, int whence);
extern int file_close(int fd);

static const char article6[] = {
    "Article. VI.\n\n"
    "All Debts contracted and Engagements entered into, before the Adoption "
    "of this Constitution, shall be as valid against the United States under "
    "this Constitution, as under the Confederation.\n\n"
    "This Constitution, and the Laws of the United States which shall be made "
    "in Pursuance thereof; and all Treaties made, or which shall be made, "
    "under the Authority of the United States, shall be the supreme Law of the "
    "Land; and the Judges in every State shall be bound thereby, any Thing in the "
    "Constitution or Laws of any State to the Contrary notwithstanding.\n\n"
    "The Senators and Representatives before mentioned, and the Members of "
    "the several State Legislatures, and all executive and judicial Officers, "
    "both of the United States and of the several States, shall be bound by Oath or "
    "Affirmation, to support this Constitution; but no religious Test shall ever "
    "be required as a Qualification to any Office or public Trust under the United States." };
    
static const char article5[] = {
    "Article. V.\n\n"
    "The Congress, whenever two thirds of both Houses shall deem it necessary, "
    "shall propose Amendments to this Constitution, or, on the Application of the "
    "Legislatures of two thirds of the several States, shall call a Convention "
    "for proposing Amendments, which, in either Case, shall be valid to all Intents "
    "and Purposes, as Part of this Constitution, when ratified by the Legislatures "
    "of three fourths of the several States, or by Conventions in three fourths thereof, "
    "as the one or the other Mode of Ratification may be proposed by the Congress; "
    "Provided that no Amendment which may be made prior to the Year One thousand eight "
    "hundred and eight shall in any Manner affect the first and fourth Clauses "
    "in the Ninth Section of the first Article; and that no State, without its Consent, "
    "shall be deprived of its equal Suffrage in the Senate."
};


static int seekset[] = {3,14,4096,159,4095,-2010,26,-1,53,5,89,5000,20000,79,323,0,4097, sizeof(article5), 52, sizeof(article5)-1 };
static int seekend[] = { 3, 14,  159,  20000,  5020,  1,  4096,  409,  4095, 20000,  100,  4097,  100,  5906, 0, 16,  sizeof(article5), 20, sizeof(article5)+1, 50, sizeof(article5)-1, 77, sizeof(article5)-2};
static int seekcur[] = {-10, 50, -13, 1000, -4000, 4000, -4099, 4099, 0, -1, 20000, -20000,
        sizeof(article5)};

int readFully(int fd, char buf[], int num){
    int total=0;
    char* p = buf;
    while(num>0){
        int nr = file_read(fd,p,num);
        if( nr == 0 )
            break;
        if( nr < 0 )
            return nr;
        p += nr;
        num -= nr;
        total += nr;
    }
    return total;
}
        
        
void sweet(){
    char buf[33];
    char buf2[33];
    int fd;
    int i,j,k;
    int rv;
    
    int a6len;
    for(a6len=0;article6[a6len];a6len++){}
    int a5len;
    for(a5len=0;article5[a5len];a5len++){}
    
    if( a6len != 949 ){
        kprintf("article6 had wrong length: %d\n",a6len );
        return;
    }
    if( a5len != 850 ){
        kprintf("article5 had wrong length: %d\n",a5len );
        return;
    }
    
    {
        //adler32 checksum
        unsigned short v1=1;
        unsigned short v2=0;
        const unsigned char* p = (const unsigned char*)article6;
        while(*p){
            v1 = (v1 + *p) % 65521;
            v2 = (v2 + v1) % 65521;
            p++;
        }
        unsigned sum = (v2 << 16) | v1;
        if( sum != 2129745098 ){
            kprintf("article6 sum mismatch: %d\n",sum);
            return;
        }
        v1=v2=0;
        p = (const unsigned char*)article5;
        while(*p){
            v1 = (v1 + *p) % 65521;
            v2 = (v2 + v1) % 65521;
            p++;
        }
        sum = (v2 << 16) | v1;
        if( sum != 1624781340 ){
            kprintf("article5 sum mismatch: %d\n",sum);
            return;
        }  
    }
    
    //additional tests
    int fd1,fd2,fd3;
    fd1 = file_open("article6.txt",0);
    fd2 = file_open("article4.txt",0);
    fd3 = file_open("article6.txt",0);
    if( fd1 < 0 || fd2 < 0 || fd3 < 0 ){
        kprintf("Could not open correctly!");
        kprintf("%d %d %d", fd1, fd2, fd3);
        return;
    }
    
    int theyAllMatch=1;
    while(1){
        buf[27] = 0x42;
        int nr1 = readFully(fd1,buf,27);
        if( buf[27] != 0x42 ){
            kprintf("OVERFLOW!\n");
            return;
        }
        if( nr1 < 0 ){
            kprintf("readFully wrong\n");
            return;
        }
        if( nr1 == 0 )
            break;
        buf2[27] = 0x43;
        int nr2 = readFully(fd2,buf2,27);
        if( buf2[27] != 0x43 ){
            kprintf("OVERFLOW!\n");
            return;
        }
        if( nr2 <= 0 ){
            kprintf("wrongedy wrong wrong wrong\n");
            return;
        }
        for(i=0;i<nr1;++i){
            if( buf[i] != buf2[i] )
                theyAllMatch=0;
        }
        buf2[27] = 0x32;
        int nr3 = readFully(fd3,buf2,27);
        if( buf2[27] != 0x32 ){
            kprintf("OVERFLOW!\n");
            return;
        }

        if( nr1 != nr3 ){
            kprintf("so very wrong\n");
            return;
        }
        
        //KJ Test
        //buf and buf2 are both empty
        kprintf("%s %s", buf, buf2);

        for(i=0;i<nr1;++i){
            if( buf[i] != buf2[i] ){
                kprintf("wrong! Wrong! WRONG!\n");
                return;
            }
        }
    }

    if( theyAllMatch ){
        kprintf("They all match! It's a conspiracy, I tell you!\n");
        return;
    }
    
    if( 0 != file_close(fd1) || 0 != file_close(fd2) || 0 != file_close(fd3) ){
        kprintf("A close call...\n");
        return;
    }
    
    
    fd = file_open("article6.txt",0);
    if( fd < 0 ){
        kprintf("Could not open\n");
        return;
    }
    
    const char* p = article6;
    int totalNumRead=0;
    while(1){
        buf[23] = 0x77;
        int nr = file_read(fd,buf,23);
        if( buf[23] != 0x77 ){
            kprintf("OVERFLOW");
            return;
        }
        if( nr < 0 || nr > 23){
            kprintf("Error: nr=%d\n",nr);
            return;
        }
        if( nr == 0 )
            break;
        for(i=0;i<nr;++i){
            if( *p != buf[i] ){
                kprintf("Error: Did not read correctly: At buf[%d]: Found %c, expected %c\n",
                    i,buf[i],*p);
                return;
            }
            p++;
            totalNumRead++;
            if( totalNumRead > sizeof(article6)-1 ){
                kprintf("Read too much: Read %d, expected %d at line %d\n",
                    totalNumRead, sizeof(article6)-1,__LINE__);
                return;
            }
        }
    }
    if( sizeof(article6)-1 != totalNumRead){
        kprintf("Didn't read correct amount: Expected %d, got %d at line %d\n",
            sizeof(article6)-1, totalNumRead, __LINE__);
        return;
    }
    file_close(fd);

    fd = file_open("article5.txt",0);
    if(fd<0){
        kprintf("Could not open article5.txt\n");
        return;
    }
    
    //////////////set
    for(i=0;i<sizeof(seekset)/sizeof(seekset[0]);i++){
        rv = file_seek(fd,seekset[i],SEEK_SET);
        if( seekset[i] < 0 ){
            if(rv >= 0 ){
                kprintf("Error: seek set succeeded with negative offset\n");
                return;
            }
        }
        else{
            if( rv < 0 ){
                kprintf("Seek set failed with positive offset\n");
                return;
            }
            else if( rv != seekset[i] ){
                kprintf("Seek set returned wrong value\n");
                return;
            }
            rv = file_read(fd,buf,32);
            if( seekset[i] >= sizeof(article5)-1 ){
                if( rv != 0 ){
                    kprintf("Read data, but should not have\n");
                    return;
                }
            }
            else{
                if( rv < 0 ){
                    kprintf("Read failed, but should not have\n");
                    return;
                }
                if( rv == 0 ){
                    kprintf("Read gave eof, but it should not have\n");
                    return;
                }
                if( seekset[i] + rv > sizeof(article5)-1 ){
                    kprintf("Returned too much data\n");
                    return;
                }
                for(j=0;j<rv;++j){
                    if( article5[seekset[i]+j] != buf[j] ){
                        kprintf("Read wrong contents on seek set\n");
                        return;
                    }
                }
            }
        }
    }
    
    
    ///////////////end
    for(i=0;i<sizeof(seekend) / sizeof(seekend[0]);i++){
        int sign;
        for(sign=-1;sign<=1;sign+=2){
            int offset = sizeof(article5)-1 + sign * seekend[i];
            rv = file_seek(fd,sign*seekend[i],SEEK_END);
            if( offset < 0 ){
                if(rv >= 0 ){
                    kprintf("Bad: seek end negative improperly succeeded\n");
                    return;
                }
            }
            else{
                if( rv < 0 ){
                    kprintf("seek end failed improperly\n");
                    return;
                }
                if( rv != offset ){
                    kprintf("seek end returned wrong value\n");
                    return;
                }
                rv = file_read(fd,buf,32);
                int max = sizeof(article5)-1 - offset;
                if( max < 0 )
                    max = 0;
                if( rv < 0 || rv > max ){
                    kprintf("Bad: Read wrong amount: %d; expected no more than %d\n",
                        rv,max);
                    return;
                }
                else if( rv == 0 && max != 0 ){
                    kprintf("Didn't read anything\n");
                    return;
                }
                
                for(j=0;j<rv;++j){
                    if( article5[offset+j] != buf[j] ){
                        kprintf("Read wrong contents on seek end\n");
                        return;
                    }
                }
            }
        }
    }    


    //////////cur
    for(i=0;i<sizeof(seekset)/sizeof(seekset[0]);i++){
        int m = sizeof(seekcur)/sizeof(seekcur[0]);
        for(j=0;j<m;++j){
            rv = file_seek(fd,seekset[i],SEEK_SET);
            if( seekset[i] < 0 ){
                if(rv>=0){
                    kprintf("Improperly succeeded on seek set to negative\n");
                    return;
                }
            }
            else{
                if( rv < 0 ){
                    kprintf("Failed cur/set on positive offset\n");
                    return;
                }
                if( rv != seekset[i] ){
                    kprintf("Wrong result on seek set before seek cur\n");
                    return;
                }
                
                rv = file_read(fd,buf,1);
                if( rv < 0 ){
                    kprintf("Read failed\n");
                    return;
                }
                if( seekset[i] >= sizeof(article5)-1){
                    if(rv != 0 ){
                        kprintf("Read got data but should not have\n");
                        return;
                    }
                }
                else{
                    if( rv != 1 ){
                        kprintf("Read 1 byte failed");
                        return;
                    }
                    if( article5[seekset[i]] != buf[0] ){
                        kprintf("Got wrong byte\n");
                        return;
                    }

                    rv = file_seek(fd,seekcur[j],SEEK_CUR);
                    if( seekset[i] + seekcur[j] +1 < 0 ){
                        if( rv >= 0 ){
                            kprintf("Bad: Seek to negative succeeded\n");
                            return;
                        }
                    }
                    else{
                        if( rv != seekset[i]+seekcur[j]+1 ){
                            kprintf("Seek to positive gave wrong return value: "
                                "%d expected %d\n", rv, seekset[i]+seekcur[j]);
                            return;
                        }
                        rv = file_read(fd,buf,sizeof(buf));
                        if( seekset[i] + seekcur[j] + 1 >= sizeof(article5)-1){
                            if( rv != 0 ){
                                kprintf("Read succeeded but should not have\n");
                                return;
                            }
                        }
                        else{
                            int numleft = sizeof(article5)-1 - (seekset[i]+seekcur[j]+1) ;
                            if( rv > sizeof(buf) || rv > numleft || rv == 0 ){
                                kprintf("Bad number read\n");
                                return;
                            }
                            for(k=0;k<rv;++k){
                                if( article5[seekset[i]+seekcur[j]+k+1] != buf[k] ){
                                    kprintf("Read wrong contents on seek cur: Expected %c, got %c\n",
                                        article5[seekset[i]+seekcur[j]+k+1], buf[k]);
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    if( 0 != file_close(fd) ){
        kprintf("Could not close\n");
        return;
    }
    
    kprintf("All OK\n");
}
