#pragma once
#include <cassert>
#include "stdio.h"
#include "stdint.h"

/**
 * Some code is ripped from
 * https://github.com/brendena/pico_drag_n_drop_programmer by Brenden Adamczak.
 * It is the exact specifications for FAT16 that is compatible with the Windows
 * Operating System.
 */



namespace fat {
	/**
	 * Thanks to http://elm-chan.org/docs/fat_e.html
	 * This is the nitty-gritty of the first section of FAT16. It mostly contains 
	 * general information. 
	 */ 
	struct __attribute__((packed)) BootSector {
		uint8_t     boot_jump[3];		// 0xEB, 0x3C, 0x90 will bring you to bootcode
		uint8_t     oem_name[8];		// "MSWIN 4.1" or "MSDOS 5.0"
		uint16_t    sector_size;		// Size of each sector, in bytes; Usually 512
		uint8_t     cluster_size;		// # of Sectors per Cluster
		uint16_t    reserved_sectors;	// 
		uint8_t     fat_copies;			// This HAS to be 2 due to compatability reasons
		uint16_t    root_dir_entries;	// # of dir entries in root dir; Usually 512
		uint16_t    total_sec_16;		// Unused for our purposes
		uint8_t     media_type;			// Type of disk this iis
		uint16_t    fat_table_size;		// # of Sector occupied by a single FAT
		uint16_t    sec_per_trk;		// HDD Only
		uint16_t    num_heads;			// HDD Only
		uint32_t    hidd_sec;			// Set it to 0 for legacy purposes
		uint32_t    total_sec_32;		// Total # of sectors the whole volume contains
		uint8_t		drive_num;			// For some reason only 0x00 works?
		uint8_t		reserved;			// 
		uint8_t		boot_sig;			// Used to confirm next 3 fields are present
		uint32_t	volume_id;			// Volume Serial Number
		uint8_t		volume_label[11];	// Should match label in Root Directory
		uint8_t		filesys_type[8];	// "FAT16   "
		uint8_t		bootcode[448];		// Code for a bootloader, though this is 0'd
		uint16_t	bootsign;			// Mark end of boot section
	};

	// FAT16 Needs Two of these
	// #1 and #2 are meant to be identical with error checking code to see
	// if one is corrupted, but this implemention will not cover that.
	struct FAT {
		uint16_t entry[1024];
	};

	/**
	 * Thanks to https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html
	 *
	 * This is what a directory looks like in FAT. There exists one "Root"
	 * directory that is the mother of all other directories.
	 */
	struct __attribute__((packed)) DirectoryEntry {
		char name[8];
		char extension[3];
		uint8_t attributes;
		uint8_t reserved;
		uint8_t createTimeFine;
		uint16_t createTime;
		uint16_t createDate;
		uint16_t lastAccessDate;
		uint16_t highStartCluster;
		uint16_t updateTime;
		uint16_t updateDate;
		uint16_t startCluster;
		uint32_t size;
	};
	
	static_assert(sizeof(DirectoryEntry) == 32, "Directory Entry size is not right");
}



