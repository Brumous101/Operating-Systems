#include "disk.h"
#include "kprintf.h"
#include "errno.h"
#include "utility.h"

#define MMCP_START ((volatile unsigned*) 0x1c000000 )
#define POWER ( MMCP_START)
#define CLOCK (MMCP_START+1)
#define ARG (MMCP_START+2)
#define CMD (MMCP_START+3)
#define RESPONSE_CMD (MMCP_START+4)
#define RESPONSE (MMCP_START+5)
#define DATA_TIMER (MMCP_START+9)
#define DATA_LENGTH (MMCP_START+10)
#define DATA_CONTROL (MMCP_START+11)
#define DATA_COUNTER (MMCP_START+12)
#define STATUS (MMCP_START+13)
#define CLEAR (MMCP_START+14)
#define INTERRUPT0_MASK (MMCP_START+15)
#define INTERRUPT1_MASK (MMCP_START+16)
#define SELECT (MMCP_START+17)
#define FIFO_COUNT (MMCP_START+18)
#define DATA_FIFO (MMCP_START+32)

//We are pushing this struct in a stack that will not autofill the empty space between bytes with other code.
//It's less efficient but it's so we know exactly where each piece of data is because otherwise the bytes would
//auto fill with the space in between the data types.
#pragma pack(push,1)
struct VBR{
    char jmp[3];
    char oem[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char num_fats;
    unsigned short UNUSED_num_root_dir_entries;
    unsigned short UNUSED_num_sectors_small;
    unsigned char id ;
    unsigned short UNUSED_sectors_per_fat_12_16;
    unsigned short sectors_per_track;
    unsigned short num_heads;
    unsigned int first_sector;
    unsigned int num_sectors;
    unsigned int sectors_per_fat;
    unsigned short flags;
    unsigned short version;
    unsigned int root_cluster;
    unsigned short fsinfo_sector;
    unsigned short backup_boot_sector;
    char reservedField[12];
    unsigned char drive_number;
    unsigned char flags2;
    unsigned char signature;
    unsigned int serial_number;
    char label[11];
    char identifier[8];
    char code[420];
    unsigned short checksum;
};
#pragma pack(pop)

#pragma pack(push,1)
struct DirEntry {
    char base[8];
    char ext[3];
    unsigned char attributes;
    unsigned char reserved;
    unsigned char creationTimeSecondsTenths;
    unsigned short creationTime;
    unsigned short creationDate;
    unsigned short lastAccessDate;
    unsigned short clusterHigh;
    unsigned short lastModifiedTime;
    unsigned short lastModifiedDate;
    unsigned short clusterLow;
    unsigned int size;
};
#pragma pack(pop)

#pragma pack(push,1)
struct LFNEntry {
    unsigned char sequenceNumber;
    char name0[10];
    char attribute; //always 15
    char zero;      //always zero
    char checksum;
    char name1[12];
    unsigned short alsozero;    //always zero
    char name2[4];
};
#pragma pack(pop)

struct File{
    int flags;
    int in_use;
    int offset;
    int size;
    int firstCluster;
    int clusterOffset;
};
#define MAX_FILES 16

//512 is the size of the sector, you can count the size of all the data types individually and you will see its 512

static char clusterbuffer[4096];
struct VBR vbr; //VBR is the name of the structure, vbr is the variable name
struct File fileTable[MAX_FILES];
struct File * fp;
struct DirEntry* de = (struct DirEntry*) &(clusterbuffer[0]);
struct LFNEntry* le = (struct LFNEntry*) &(clusterbuffer[0]);


void disk_init(){
    *POWER = 3;
    *CLOCK = 8;
    *CMD= (1<<10);
    do{
        *CMD = 55 | (1<<10) | (1<<6);
        *ARG = 0xffffffff;
        *CMD = 41 | (1<<10) | (1<<6);
        } while(isBusy());
    *CMD = 2 | (1<<10) | (1<<6) | (1<<7);
    *CMD = 3 | (1<<10) | (1<<6) ;
    unsigned relative_address = *RESPONSE;
    *ARG = relative_address;
    *CMD = 7 | (1<<10) | (1<<6);

    disk_read_sector(0, &vbr);
    kprintf("%s\n", vbr.label);

    for(int i=0; i < MAX_FILES;i++){
        fileTable[i].in_use = 0;
        fileTable[i].flags = 0;
        fileTable[i].offset = 0;
        fileTable[i].size = 0;
    }
    fileTable[0].in_use = 1;//keyboard
    fileTable[1].in_use = 1;//screen
    fileTable[2].in_use = 1;//screen but not buffered
    //kprintf("123 %d 321", vbr.sectors_per_cluster);
}

int isBusy(){
    //return busy bit
    return *STATUS & (1<<24) ;
}

void disk_read_sector(unsigned sector, void* datablock){
    //JH
    //kprintf("READ SECTOR %d\n",sector);


    *DATA_TIMER = 100;
    *DATA_LENGTH = 512;
    *DATA_CONTROL = 1 | (1<<1) | (9<<4);
    *ARG = 512*sector;
    *CLEAR=0x3ff; // clears status flags
    *CMD = 17 | (1<<10) | (1<<6);
    unsigned* p = (unsigned*)datablock;
    int k;
    for(k=0;k<128;++k){
        while( *STATUS & (1<<19) )
            ;   //the FIFO is empty
        *CLEAR = 0x3ff;
        unsigned v = *DATA_FIFO;

        //JH
        //char* x = (char*) &v;
        //kprintf("%c%c%c%c",x[0],x[1],x[2],x[3]);

        *p = v;
        p++;
    }
}

void disk_write_sector(unsigned sector, void* datablock){
    *DATA_LENGTH = 512;
    *DATA_TIMER = 100;
    *DATA_CONTROL = 1 | (9<<4);
    *ARG = 512*sector;
    *CLEAR=0x3ff; //clear status flags
    *CMD = 24 | (1<<10) | (1<<6);
    unsigned* p = (unsigned*)datablock;  //data to write
    int k;
    for(k=0;k<128;++k){
        //wait until no more pending data
        while( (*STATUS & (1<<20)) )
            ;
        *CLEAR = 0x3ff;
        *DATA_FIFO = *p;
        p++;
    }
}

unsigned clusterNumberToSectorNumber( unsigned clnum )
{
    //kprintf("first_sector %d, reserved_sectors %d, num_fats %d, spf %d, clnum %d", vbr.first_sector, vbr.reserved_sectors, vbr.num_fats, vbr.sectors_per_fat, clnum);
    return vbr.first_sector + vbr.reserved_sectors + (vbr.num_fats * vbr.sectors_per_fat) + ( (clnum - 2) * 8);
}

void readCluster( unsigned clnum, void* p)
{
    //kprintf("%d", vbr.sectors_per_cluster);   
    unsigned sectorNum = clusterNumberToSectorNumber( clnum );
    //kprintf("%d", sectorNum);
    for(int i = 0; i < vbr.sectors_per_cluster; i++){
        disk_read_sector((sectorNum + i), (char *)p + (512 * i));
    }
}

void directories(){
    readCluster(2, clusterbuffer);
    for(int j=0; de[j].base[0]; j++){
        if(de[j].attributes == 15)
        {
            //What we need to do is find a directory with a sequence number of 1, then go backwards till mask is true
            //if you have 1 lfn entry it would be like 41
            le= (struct LFNEntry*)de;
            int lfnCounter = j;
            for( ; (le[lfnCounter].sequenceNumber & 0x3f) != 1; lfnCounter++){

            }
            j = lfnCounter;
            while((0b01000000 & le[lfnCounter].sequenceNumber) != 0b01000000){
                for(int i = 0; i< 10; i+=2)
                    kprintf("%c",le[lfnCounter].name0[i]);
                for(int i = 0; i< 12; i+=2)
                    kprintf("%c",le[lfnCounter].name1[i]);
                for(int i = 0; i< 4; i+=2)
                    kprintf("%c",le[lfnCounter].name2[i]);
                lfnCounter--;
            }
            //this is the the final part of the lfn
            int flag=0;
            for(int i = 0; flag==0 && i< 10; i+=2){
                if(le[lfnCounter].name0[i] == 0)
                    flag = 1;
                else
                    kprintf("%c",le[lfnCounter].name0[i]);
            }
            for(int i = 0; flag==0 && i< 12; i+=2){
                if(le[lfnCounter].name1[i] == 0)
                    flag = 1;
                else
                    kprintf("%c",le[lfnCounter].name1[i]);
            }
            for(int i = 0; flag==0 && i< 4; i+=2){
                if(le[lfnCounter].name2[i] == 0)
                    flag = 1;
                else
                    kprintf("%c",le[lfnCounter].name2[i]);
            }
            j++;
            //YYYY YYYM MMMD DDDD
            //HHHH HMMM MMMS SSSS
            kprintf(" %d-",((de[j].creationDate&  0b1111111000000000)>> 9)+1980);

            if(((de[j].creationDate&0b0000000111100000) >> 5) < 10)
                kprintf("0");
            kprintf("%d-",(de[j].creationDate&0b0000000111100000) >> 5);

            if((de[j].creationDate& 0b0000000000011111) < 10)
                kprintf("0");
            kprintf("%d ",(de[j].creationDate& 0b0000000000011111));

            int hour = ((de[j].creationTime& 0b1111100000000000)>> 11);

            
            if(hour < 10)
                kprintf("0");
            kprintf("%d:",hour);

            if(((de[j].creationTime& 0b0000011111100000)>> 5) < 10)
                kprintf("0");
            kprintf("%d:",(de[j].creationTime& 0b0000011111100000)>> 5);

            if(((de[j].creationTime& 0b0000000000011111)) < 10)
                kprintf("0");
            kprintf("%d",((de[j].creationTime<< 1)& 0b0000000000011111));
            //points to sfn
            //check for end of the extension
        } 
        else //sfn
        {
                if(de[j].attributes != 15){
                    //change space to period
                    for ( int i=0; i < 8; ++i ){
                        if(de[j].base[i] == 32){
                            break;
                        }
                        else{
                            kprintf("%c", de[j].base[i]);
                        }
                    }
                    kprintf(".%.3s",de[j].ext);
                    //YYYY YYYM MMMD DDDD
                    //HHHH HMMM MMMS SSSS
                    kprintf(" %d-",((de[j].creationDate&  0b1111111000000000)>> 9)+1980);

                    if(((de[j].creationDate&0b0000000111100000) >> 5) < 10)
                        kprintf("0");
                    kprintf("%d-",(de[j].creationDate&0b0000000111100000) >> 5);

                    if((de[j].creationDate& 0b0000000000011111) < 10)
                        kprintf("0");
                    kprintf("%d ",(de[j].creationDate& 0b0000000000011111));

                    int hour = ((de[j].creationTime& 0b1111100000000000)>> 11);

                    
                    if(hour < 10)
                        kprintf("0");
                    kprintf("%d:",hour);

                    if(((de[j].creationTime& 0b0000011111100000)>> 5) < 10)
                        kprintf("0");
                    kprintf("%d:",(de[j].creationTime& 0b0000011111100000)>> 5);

                    if(((de[j].creationTime& 0b0000000000011111)) < 10)
                        kprintf("0");
                    kprintf("%d",((de[j].creationTime<< 1)& 0b0000000000011111));
                }
        }
        kprintf("\n");
    }
}

int file_open(const char* fname, int flags){
    //kprintf("%s", fname);
    //does file exist (scan root directory), permissions, file table full?
    //get_files();
    //is this a file?
    readCluster(2, clusterbuffer);
    //kprintf("//////////////////////\n");
    //kprintf("%d \n", clusterbuffer[0]);
    //kprintf("//////////////////////\n");
    int j = 0;
    int k = 0;
    int l = 0;
    char file[13];
    for(int h = 0; de[h].base[0]; h++){
        get_file(file, h);//This will format the string at a directory and apply it to the "file[13]" variable
        k = stringEquals(file,fname);
        if(k){
            //kprintf("%s\n", file);
            //kprintf("%s\n", fname);
            //kprintf("%d\n", k);
            //kprintf("%d\n", h);
            l = h -1;
            //kprintf("\nMy l value that denotes which file index it is: %d\n", l);
            break;
        }
    }
    //it finds all file names
    //kprintf("%d\n", k);
    if(k==0)
        return -ENOENT;
    for(int i = 0; i<=MAX_FILES; i++){
        if(j ==0 && i == MAX_FILES){
            return -EMFILE;
        }
        //The problem is the index in the file table when given the same file name!
        else if(j ==0 && fileTable[i].in_use == 0){
            j = 1;
            fileTable[i].in_use = 1;
            fileTable[i].flags = flags;
            fileTable[i].offset = 0;
            fileTable[i].size = de[l+1].size;
            fileTable[i].firstCluster = ((de[l+1].clusterHigh << 16) | (de[l+1].clusterLow));
            //kprintf("%d\n",i);
            //kprintf("i = %d files inuse %d\n", i, fileTable[i].in_use);
            return i;
        }
        
    }
    return -1;
}

int file_close(int fd){
    readCluster(2, clusterbuffer);
    if(fd < 0 && fd >= MAX_FILES){
        return -1;
    }
    if(fileTable[fd].in_use == 1){
        fileTable[fd].in_use = 0;
        fileTable[fd].flags = 0;
        fileTable[fd].offset = 0;
        fileTable[fd].size = 0;
        fileTable[fd].firstCluster = 0;
        return SUCCESS;
    }
    return -1;
}

int stringEquals(const char* s1,const char* s2){
    for(int i=0; (s1[i] != 0) || (s2[i] != 0); i++){
        char c1 = s1[i];
        char c2 = s2[i];
        if(c1 >= 'a' && c1 <= 'z')
            c1 -= 32;//ASCII lowercase and capital letters are 32 apart (a = 97, A =65)
        //compare c1 and c2
        if(c2 >= 'a' && c2 <= 'z')
            c2 -= 32;//ASCII lowercase and capital letters are 32 apart (a = 97, A =65)
        if(c1 != c2)
            return 0;
        if((s1[i] == 0) && (s2[i] != 0))
            return 0;
    }
    return 1;
}

void get_file(char file[], int j){/*the caller needs to send file[13]*/
    //You need to pass the j into this and then this will change the parameter that us being passed. you cant return it as a val in c
    readCluster(2, clusterbuffer);
    //char file[13];
    if(de[j].attributes != 15){
        int i, h;
        for ( i = 0; i < 8; i++ ){
            if(de[j].base[i] == 32){
                break;
            }
        }
        for ( h = 0; h < 3; h++ ){
            if(de[j].ext[h] == 32){
                break;
            }
        }
        //sizeof is a compile time operator we would need a runtime operator
        kmemcpy(file, de[j].base, i);
        file[i]='.';
        kmemcpy(&file[i+1], de[j].ext, h);
        file[i+h+1] ='\0';//'\0' is the same as file[i+h+1] = 0
    }
}

int file_read(int fd, void* buf, int count){
    //it's failing on null buffer on the double read time!!!
    //kprintf("\nfd passed %d\n",fd);
    fp = &fileTable[fd];
    int bytesLeftInFile= fp->size - fp->offset;

    int numToCopy = 0;
    short BYTES_PER_CLUSTER = vbr.bytes_per_sector *  vbr.sectors_per_cluster;
    unsigned remaining = BYTES_PER_CLUSTER - fp->offset;
    //int lengthOfFile = (fp -> offset / 4095) + (fp -> offset % 4095);
    /*
    kprintf("\nlengthOfFile: %d\n",lengthOfFile);
    kprintf("fp-offset %d\n", fp->offset);
    kprintf("buf %d\n", sizeof(buf));
    kprintf("numToCopy %d\n", numToCopy);
    kprintf("bytesLeftInFile %d\n", bytesLeftInFile);
    kprintf("count %d\n", count);
    kprintf("remaining %d\n", remaining);
    kprintf("bytesLeftInFile %d\n", bytesLeftInFile);
    */
    if( (count == 0))
        return 0;
    if(count < 0){
        return -1;
    }
    if(bytesLeftInFile < 0){
        return 0;
    }
        
    readCluster(fp->firstCluster + fp->offset / 4095, clusterbuffer);

    //which is smallest?
    numToCopy = ( remaining < count ) ? remaining : count;
    numToCopy = (numToCopy < bytesLeftInFile) ? numToCopy : bytesLeftInFile;
    kmemcpy( buf, clusterbuffer + fp->offset , numToCopy );
    //kprintf("\nPrinting buf:\n");
    /*
    for(int j = 0; j < numToCopy; ++j)
        kprintf("%c", ((char*) buf)[j]);
    */
    fp->offset += numToCopy;
    //if(numToCopy ==0)
    //    fp->offset =0;
    //int test = fp->offset;
    return numToCopy;
}

int file_write(int fd, const void* buf, int count){
    return -ENOSYS; //no such system call
}

int file_seek(int fd, int offset, int whence){
    fp = &fileTable[fd];
    if(whence == 0){//SEEK_SET
        if(offset < 0)
            return -EINVAL;
        else
            fp->offset = offset;
    }
    else if(whence == 1){//SEEK_CUR
        if(fp->offset + offset < 0)
            return -EINVAL;
        else
            fp->offset += offset;
    }
    else if(whence == 2){//SEEK_END
        if(fp->size + offset < 0)
            return -EINVAL;
        else
            fp->offset = fp->size + offset;
    }
    else//No such Whence
        return -EINVAL;
    return fp->offset;
}