#include "disk.h"
#include "kprintf.h"

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

//512 is the size of the sector, you can count the size of all the data types individually and you will see its 512

struct VBR vbr; //VBR is the name of the structure, vbr is the variable name


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
}

int isBusy(){
    //return busy bit
    return *STATUS & (1<<24) ;
}

void disk_read_sector(unsigned sector, void* datablock){
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
    unsigned sectorNum = clusterNumberToSectorNumber( clnum );
    //kprintf("%d", sectorNum);
    for(int i = 0; i < vbr.sectors_per_cluster; i++){
        disk_read_sector((sectorNum + i), (char *)p + (512 * i));
    }
}
static char clusterbuffer[4096];
struct DirEntry* de = (struct DirEntry*) &(clusterbuffer[0]);
struct LFNEntry* le = (struct LFNEntry*) &(clusterbuffer[0]);

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