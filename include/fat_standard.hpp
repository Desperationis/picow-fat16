#pragma once
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include <string>
#include "util.h"


namespace fat {

/**
 * Thanks to http://elm-chan.org/docs/fat_e.html and
 * https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html This is the nitty-gritty
 * of the first section of FAT16. It mostly contains general information.
 */

struct __attribute__((packed)) BootSector {
	uint8_t boot_jump[3];      // 0xEB, 0x3C, 0x90 will bring you to bootcode
	uint8_t oem_name[8];       // "MSWIN 4.1" or "MSDOS 5.0"
	uint16_t sector_size;      // Size of each sector, in bytes; Usually 512
	uint8_t cluster_size;      // # of Sectors per Cluster
	uint16_t reserved_sectors; //
	uint8_t fat_copies;        // This HAS to be 2 due to compatability reasons
	uint16_t root_dir_entries; // # of dir entries in root dir; Usually 512
	uint16_t total_sec_16;     // Unused for our purposes
	uint8_t media_type;        // Type of disk this iis
	uint16_t fat_table_size;   // # of Sector occupied by a single FAT
	uint16_t sec_per_trk;      // HDD Only
	uint16_t num_heads;        // HDD Only
	uint32_t hidd_sec;         // Set it to 0 for legacy purposes
	uint32_t total_sec_32;     // Total # of sectors the whole volume contains
	uint8_t drive_num;         // For some reason only 0x00 works?
	uint8_t reserved;          //
	uint8_t boot_sig;          // Used to confirm next 3 fields are present
	uint32_t volume_id;        // Volume Serial Number
	uint8_t volume_label[11];  // Should match label in Root Directory
	uint8_t filesys_type[8];   // "FAT16   "
	uint8_t bootcode[448];     // Code for a bootloader, though this is 0'd
	uint16_t bootsign;         // Mark end of boot section
};


/**
 * Thanks to https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#FAT
 *
 * FAT16 Needs Two of these
 * #1 and #2 are meant to be identical with error checking code to see
 * if one is corrupted, but this implemention will not cover that.
 */
struct FATTable {
public:
	FATTable() {
		memset(entries, 0, sizeof(entries));
	}

	/**
	 * Each entry corresponds to a cluster. For example, Entry #17 is
	 * Cluster #17, Entry #0 is Cluster #0, etc. The value in this index
	 * tells the next cluster to look at. If 0xFF, it ends the "chain",
	 * allowing you to form a file from multiple clusters. If a cluster
	 * starts with 0xFF, then the file is only one cluster long, for
	 * example.
	 */
	void Set(uint16_t cluster, uint16_t next_cluster) {
		entries[cluster] = next_cluster;
	}

	uint16_t* GetBytes() {
		return entries;
	}


private:
	// Our FAT only has 129 entries, look at boot entry.
	//
	// Just like BootSector, memcpy converts this to little
	// endian automatically.
	uint16_t entries[129];
};

/**
 * Thanks to https://averstak.tripod.com/fatdox/dir.htm 
 * and http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html
 *
 * This is what a directory looks like in FAT16. 
 */
struct __attribute__((packed)) DirectoryEntry {
	char name[8];
	char extension[3];
	uint8_t attributes;
	uint8_t reserved;
	uint8_t create_time_ms;
	uint16_t create_time;
	uint16_t create_date;
	uint16_t last_access_date;
	uint16_t reserved32; // High Cluster only for FAT32
	uint16_t update_time;
	uint16_t update_date;
	uint16_t start_cluster;
	uint32_t size;
};

class DirectoryEntryBuilder {
public:
	// Bit flags
	enum ATTR {
		READ_ONLY = 0x01,
		HIDDEN = 0x02,
		SYSTEM = 0x04,
		VOLUME_LABEL = 0x08,
		DIRECTORY = 0x10,
		ARCHIVE = 0x20
	};

public:
	DirectoryEntryBuilder() = default;

	bool SetName(std::string name, std::string extension) {
		if (name.size() != 8)
			return false;

		if (extension.size() != 3)
			return false;

		for(size_t i = 0; i < name.size(); i++)
			entry.name[i] = name[i];
		for(size_t i = 0; i < extension.size(); i++)
			entry.extension[i] = extension[i];

		return true;
	}

	void SetAttribute(uint8_t attr) {
		entry.attributes = attr;
	}

	void SetCreateTime(uint16_t hour, uint16_t minute, uint16_t second, uint8_t ms) {
		entry.create_time_ms = ms;
		entry.create_time = CreateTime(hour, minute, second);
	}

	void SetCreateDate(uint16_t month, uint16_t day, uint16_t year) {
		entry.create_date = CreateDate(month, day, year);
	}

	void SetLastAccessDate(uint16_t month, uint16_t day, uint16_t year) {
		entry.last_access_date = CreateDate(month, day, year);
	}

	void SetUpdateTime(uint16_t hour, uint16_t minute, uint16_t second) {
		entry.update_time = CreateTime(hour, minute, second);
	}

	void SetUpdateDate(uint16_t month, uint16_t day, uint16_t year) {
		entry.update_date = CreateDate(month, day, year);
	}

	void SetStartCluster(uint16_t cluster) {
		entry.start_cluster = cluster;
	}

	void SetFileSize(uint16_t size) {
		entry.size = size;
	}

	DirectoryEntry Build() {
		entry.reserved = 0;
		entry.reserved32 = 0;

		return entry;
	}

private:
	uint16_t CreateDate(uint16_t month, uint16_t day, uint16_t year) {
		return (day & 0x1F) |
			((month & 0x0F) << 5) |
			(((year - 1980) & 0x7F) << 9);
	}

	uint16_t CreateTime(uint16_t hour, uint16_t minute, uint16_t second) {
		// For some reason, FAT16 stores seconds/2 instead of the raw value
		return (uint16_t(second / 2) & 0x1F) | 
			((minute & 0x3F) << 5) |
			((hour & 0x1F) << 11); 
	}


private:
	DirectoryEntry entry;
};


class RootDirectory {
public:
	RootDirectory() {
		current_entry = 0;
		memset(entries, 0, sizeof(entries));
	}

	void PushEntry(DirectoryEntry entry) {
		entries[current_entry] = entry;
		current_entry++;
	}

	DirectoryEntry* GetBytes() {
		return entries;
	}


private:
	DirectoryEntry entries[512];
	size_t current_entry;
};

}



