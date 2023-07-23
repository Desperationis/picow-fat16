#include "fat.h"
#include "fat16.hpp"
#include "string.h"
#include "util.h"
#include "stdio.h"
#include <string>
#include <iomanip>

Fat16::Fat16() {
	boot.boot_jump[0] = 0xEB;
	boot.boot_jump[1] = 0x3C;
	boot.boot_jump[2] = 0x90;
	
	std::string oem("MSWIN4.1");
	for(size_t i = 0; i < oem.size(); i++)
		boot.oem_name[i] = oem[i];

	// Pico memory is little endian, so Little Endian conversion is done
	// automatically on memcpy.
	boot.sector_size = uint16_t(0x0200);
	boot.cluster_size = 0x08;
	boot.reserved_sectors = int16_t(0x0001);
	boot.fat_copies = 0x02;
	boot.root_dir_entries = uint16_t(0x0200);
	boot.total_sec_16 = 0x0000;
	boot.media_type = 0xF8;
	boot.fat_table_size = uint16_t(0x0081);
	boot.sec_per_trk = uint16_t(0x0001);
	boot.num_heads = uint16_t(0x0001);
	boot.hidd_sec = uint32_t(0x00000001);
	boot.total_sec_32 = uint32_t(0x0003FFFF);
	boot.drive_num = 0x00;
	boot.reserved = 0x00;
	boot.boot_sig = 0x29;
	boot.volume_id = uint32_t(0x000B0450);

	std::string label("picowremote");
	for(size_t i = 0; i < label.size(); i++)
		boot.volume_label[i] = label[i];

	std::string type("FAT16   ");
	for(size_t i = 0; i < type.size(); i++)
		boot.filesys_type[i] = type[i];

	// Zero out all bootcode, we won't be needing it.
	memset(boot.bootcode, 0, sizeof(boot.bootcode));

	boot.bootsign = uint16_t(0xAA55);

}


void Fat16::print_reserved() {
	auto boot_sect = (fat::BootSector*)DISK_reservedSection;
    safe_print("bootOEM %.8s\n", boot_sect->oem_name);
    safe_print("cluster size %x\n",boot_sect->cluster_size);
    safe_print("fat copies %x\n", boot_sect->fat_copies);
    safe_print("fat table size %x\n", boot_sect->fat_table_size);
    safe_print("totalSec32 %x\n", boot_sect->total_sec_32);
}


/**
* Read one of the FAT sections if the LBA matches. 
*
* @return Size of bytes read. -1 is returned on error.
*/ 
int32_t Fat16::get_block(const uint32_t lba, void* buffer, uint32_t bufsize) {
	// out of space
	if ( lba >= DISK_BLOCK_NUM ) return -1;

	void* addr = 0;
	if(lba == INDEX_RESERVED)
	{
		//addr = DISK_reservedSection;
		addr = &boot;
	}

	// FAT #2 is meant to be used to have a copy of data for corruption prevention.
	else if(lba == INDEX_FAT_TABLE_1_START || lba == INDEX_FAT_TABLE_2_START)
	{
		addr = DISK_fatTable;
	}
	else if(lba == INDEX_ROOT_DIRECTORY)
	{
		addr = DISK_rootDirectory;
	}
	else if(lba >= INDEX_DATA_STARTS && lba <= INDEX_DATA_END )
	{
		//printf("lba %d, bufsize %d, offset %d\n",lba, bufsize, offset);
		//DISK_data is only one section large but the cluster sizes are 8.
		//So if there was a larger file it would be bad.
		addr = DISK_data[(lba - INDEX_DATA_STARTS) / 8];
		//printf("loading section %d\n",(lba - INDEX_DATA_STARTS) / 8);
	}
	if(addr != 0)
	{
		memcpy(buffer, addr, bufsize);
	}
	else{
		memset(buffer,0, bufsize);
	}
	return (int32_t) bufsize;
}
