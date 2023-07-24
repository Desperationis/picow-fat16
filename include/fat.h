#pragma once
#include "stdint.h"
#include "fat16.hpp"

#define HTML_Page_Data \
 R"(<html><head><meta http-equiv="refresh" content="0;URL='https://raspberrypi.com/device/RP2?version=E0C9125B0D9B'"/></head><body>Redirecting to <a href='https://raspberrypi.com/device/RP2?version=E0C9125B0D9B'>raspberrypi.com</a></body></html>)"


#define DATA_Page_Data \
 "UF2 Bootloader v3.0\n\
Model: Raspberry Pi RP2\n\
Board-ID: picowremote\n"


class Fat16 {
public:
	enum
	{
		DISK_BLOCK_NUM  = 0x3ffff, // 8KB is the smallest size that windows allow to mount
		DISK_BLOCK_SIZE = 512,
		DISK_CLUSTER_SIZE = 8
	};

// Index of places (block #)
	enum {
		INDEX_RESERVED = 0,

		// Why are there two fat tables? The second one is for "data
		// protection" in case one gets messed up.
		INDEX_FAT_TABLE_1_START = 1, // Fat table size is 0x81
		INDEX_FAT_TABLE_2_START = 0x82,
		INDEX_ROOT_DIRECTORY = 0x103,
		INDEX_DATA_STARTS = 0x123,
		INDEX_DATA_END = (0x12b + DISK_CLUSTER_SIZE - 1)
	};

public:
	Fat16();

	constexpr int get_block_count() const {
		return DISK_BLOCK_NUM;
	}

	constexpr int get_block_size() const {
		return DISK_BLOCK_SIZE;
	}

	void print_reserved();

	int32_t get_block(const uint32_t lba, void* buffer, uint32_t bufsize);


private:
	fat::BootSector boot;
	fat::FAT fat_table;
	fat::RootDirectory root_dir;

	//block size is not cluster size
	uint8_t DISK_data[2][DISK_BLOCK_SIZE] =
	{
	  {HTML_Page_Data},
	  {DATA_Page_Data}
	};
};

