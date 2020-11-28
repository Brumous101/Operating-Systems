#pragma once

void disk_init();
int isBusy();
void disk_read_sector(unsigned sector, void* datablock);
void disk_write_sector(unsigned sector, void* datablock);
unsigned clusterNumberToSectorNumber( unsigned clnum );
void readCluster( unsigned clnum, void* p);
void directories();
int file_open(const char* fname, int flags);
int file_close(int fd);
int stringEquals(const char* s1,const char* s2);
void get_file();
int file_read(int fd, void* buf, int count);
int file_write(int fd, const void* buf, int count);
int file_seek(int fd, int offset, int whence);