#pragma once

void disk_init();
int isBusy();
void disk_read_sector(unsigned sector, void* datablock);
void disk_write_sector(unsigned sector, void* datablock);