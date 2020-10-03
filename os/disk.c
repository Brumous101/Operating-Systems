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
    kprintf("%u", vbr.num_sectors);
}

int isBusy(){
    //return busy bit
    return *STATUS & (1<<24) ;
}

void disk_read_sector(unsigned sector, void* datablock){
    *DATA_TIMER = 512;
    *DATA_LENGTH = 100;
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