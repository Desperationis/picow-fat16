#include "fat.h"
#include "fat_standard.hpp"
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

	// This is the raw data that must be written in little endian to flash,
	// as FAT is little endian. Thankfully, the pico itself has memory that
	// works in little endian. Because of this, memcpy down below does this
	// conversion automatically on structs whenever data needs to be read.
	//
	// FAT type (12, 16, 32) depends solely on the number of clusters in 
	// the data section. This is determined by the following formula:
	//
	// Cluster Count = Data Sectors / Sectors per Cluster
	//
	// < 4086 Clusters ==> FAT12
	// <65524 Clusters ==> FAT16
	// >65524 Clusters ==> FAT32
	//
	// To force system to recognize this as FAT16, size must be between
	// 4086 * 8 * 512 < size < 65524 * 8 * 512, or 16mb < size < 268mb.
	// For compatibility reasons, Microsoft themselves recommend going
	// with a cluster size that isn't too close to the boundaries. This 
	// is why the Pico must fake 128mb of storage.
	boot.sector_size =		uint16_t(512);			// Standard sector size
	boot.cluster_size =		uint8_t(8);				// Compatibility (4k)
	boot.reserved_sectors = int16_t(1);				// Compatibility
	boot.fat_copies =		uint8_t(2);				// Compatibility
	boot.root_dir_entries = uint16_t(512);			// Compatibility
	boot.total_sec_16 =		uint16_t(0);			// Not used
	boot.media_type =		uint8_t(0xF8);			// Compatibility
	boot.fat_table_size =	uint16_t(129);			// 128 Clusters
	boot.sec_per_trk =		uint16_t(1);			// Compatibility
	boot.num_heads =		uint16_t(1);			// Compatibility
	boot.hidd_sec =			uint32_t(1);			// Compatibility
	boot.total_sec_32 =		uint32_t(262143);		// 128mb
	boot.drive_num =		uint8_t(0x00);			// Compatibility
	boot.reserved =			uint8_t(0x00);			// Compatibility
	boot.boot_sig =			uint8_t(0x29);			// Compatibility
	boot.volume_id =		uint32_t(0x000B0450);	// Compatibility

	std::string label("picowremote");
	for(size_t i = 0; i < label.size(); i++)
		boot.volume_label[i] = label[i];

	std::string type("FAT16   ");
	for(size_t i = 0; i < type.size(); i++)
		boot.filesys_type[i] = type[i];

	// Zero out all bootcode, we won't be needing it.
	memset(boot.bootcode, 0, sizeof(boot.bootcode));

	boot.bootsign = uint16_t(0xAA55);



	fat_table.Push(0xFFF8); // Cluster 0: FAT ID
	fat_table.Push(0xFFFF); // Cluster 1: Reserved
	fat_table.Push(0xFFFF); // Cluster 2: HTML Doc
	fat_table.Push(0xFFFF); // Cluster 3: TXT File
	

	// The first entry is a special entry that labels the
	// partition.
	fat::DirectoryEntryBuilder builder;
	builder.SetName("picowrem", "ote");
	builder.SetAttribute(builder.ARCHIVE | builder.VOLUME_LABEL);
	builder.SetCreateTime(0, 0, 0, 0);
	builder.SetCreateDate(0, 0, 1980);
	builder.SetLastAccessDate(0, 0, 1980);
	builder.SetUpdateTime(0, 0 ,0);
	builder.SetUpdateDate(0, 0, 1980);
	builder.SetStartCluster(0);
	builder.SetFileSize(0);
	root_dir.PushEntry(builder.Build());

	builder.SetName("INDEX   ", "HTM");
	builder.SetAttribute(builder.ARCHIVE | builder.READ_ONLY);
	builder.SetCreateTime(13, 42, 36, 198);
	builder.SetCreateDate(11, 5, 2013);
	builder.SetLastAccessDate(11, 5, 2013);
	builder.SetUpdateTime(13, 44, 16);
	builder.SetUpdateDate(11, 5, 2013);
	builder.SetStartCluster(2);
	builder.SetFileSize(0xF1);
	root_dir.PushEntry(builder.Build());

	builder.SetName("INFO_UF2", "TXT");
	builder.SetAttribute(builder.ARCHIVE | builder.READ_ONLY);
	builder.SetCreateTime(13, 42, 36, 198);
	builder.SetCreateDate(11, 5, 2013);
	builder.SetLastAccessDate(11, 5, 2013);
	builder.SetUpdateTime(13, 44, 16);
	builder.SetUpdateDate(11, 5, 2013);
	builder.SetStartCluster(3);
	builder.SetFileSize(4068); // Not 4069 because of \0 character
	root_dir.PushEntry(builder.Build());
}

/**
* Read one of the FAT sections if the LBA matches. 
*
* @return Size of bytes read. -1 is returned on error.
*/ 

/**
* Return the sector at LBA. This assumes the USB connection is set so that the host
* only reads in increments of 512.
*/
int32_t Fat16::GetBlock(const uint32_t lba, void* buffer, uint32_t bufsize) {
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
		addr = fat_table.GetBytes();
	}
	else if(lba == INDEX_ROOT_DIRECTORY)
	{
		addr = root_dir.GetBytes();
	}
	else if(lba >= INDEX_DATA_STARTS && lba <= INDEX_DATA_END )
	{
		//printf("lba %d, bufsize %d, offset %d\n",lba, bufsize, offset);
		//DISK_data is only one section large but the cluster sizes are 8.
		//So if there was a larger file it would be bad.
		//printf("loading section %d\n",(lba - INDEX_DATA_STARTS) / 8);

		// Figure out which sector of the cluster must be returned.
		safe_print("-----OVERRIDE COMMENCE-----\n");
		//safe_print("lun: %d\n", lun);
		safe_print("lba: %d\n", lba);
		//safe_print("offset: %d\n", offset);
		safe_print("bufsize: %d\n", bufsize);
		safe_print("--------OVERRIDE END-------\n");
		safe_print("\n");
		size_t cluster_number = (lba - INDEX_DATA_STARTS) / 8;
		size_t clus_start_sec = INDEX_DATA_STARTS + cluster_number * DISK_CLUSTER_SIZE;
		size_t offset = lba - clus_start_sec;

		addr = data[cluster_number] + offset * DISK_BLOCK_SIZE;
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
