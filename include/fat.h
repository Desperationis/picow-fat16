#pragma once
#include "stdint.h"
#include "fat16.hpp"

#define HTML_Page_Data \
 R"(<html><head><meta http-equiv="refresh" content="0;URL='https://raspberrypi.com/device/RP2?version=E0C9125B0D9B'"/></head><body>Redirecting to <a href='https://raspberrypi.com/device/RP2?version=E0C9125B0D9B'>raspberrypi.com</a></body></html>)"


#define DATA_Page_Data \
 "UF2 Bootloader v3.0\n\
Model: Raspberry Pi RP2\n\
Board-ID: picowremote\n"

#define ToLittleEndian16(val) \
	 ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

#define ToLittleEndian32(val) \
	 ( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
	   (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )

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

	uint8_t DISK_reservedSection[DISK_BLOCK_SIZE] = 
	{
		0xEB, 0x3C, 0x90,  // boot jump
		0x4D, 0x53, 0x57, 0x49, 0x4E, 0x34, 0x2E, 0x31, //MSWIN4.1
		0x00, 0x02,  //sector 512 (little endian)
		0x08,        //cluster size 8 sectors -unit for file sizes
		0x01, 0x00,  //BPB_RsvdSecCnt (little endian)
		0x02,        //Number of fat tables
		0x00, 0x02,  //BPB_RootEntCnt (little endian)
		0x00, 0x00,  //16 bit fat sector count - 0 larger then 0x10000
		0xF8,        //- non-removable disks a 
		0x81, 0x00,       //BPB_FATSz16 - Size of fat table
		0x01, 0x00, //BPB_SecPerTrk 
		0x01, 0x00, //BPB_NumHeads
		0x01, 0x00, 0x00, 0x00, //??? BPB_HiddSec 
		0xFF, 0xFF, 0x03, 0x00, //BPB_TotSec32
		0x00,  //BS_DrvNum  - probably be 0x80 but is not? 
		0x00,  // BS_Reserved
		0x29,  // BS_BootSig
		0x50, 0x04, 0x0B, 0x00, // BS_VolID
		'p' , 'i' , 'c' , 'o' , 'w' , 'r' , 'e' , 'm' , 'o' , 't' , 'e', // BS_VolLab
		'F', 'A', 'T', '1', '6', ' ', ' ', ' ',  // BS_FilSysType

		// Zero BS_BootCode, because we aren't booting
		0x00, 0x00,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

		// BS_BootSign
		0x55, 0xAA
	};
	uint8_t DISK_fatTable[DISK_BLOCK_SIZE] =
	{
		0xF8, 0xFF, // F8 FAT ID
		0xFF, 0xFF, 
		0xFF, 0xFF, //HTML doc
		0xFF, 0xFF, //Txt file
	};

	uint8_t DISK_rootDirectory[DISK_BLOCK_SIZE] = 
	{
		  // first entry is volume label
		  'p' , 'i' , 'c' , 'o' , 'w' , 'r' , 'e' , 'm' , 'o' , 't' , 'e' , 0x28, 0x00, 0x00, 0x00, 0x00,
		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0, 0x0, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		  // second entry is html file
		  'I' , 'N' , 'D' , 'E' , 'X' , ' ' , ' ' , ' ' , 'H' , 'T' , 'M' , 0x21, 0x00, 0xC6, 0x52, 0x6D,
		  0x65, 0x43, 0x65, 0x43, 0x00, 0x00, 0x88, 0x6D, 0x65, 0x43, 
		  0x02, 0x00, //cluster location
		  0xF1, 0x00, 0x00, 0x00, // html's files size (4 Bytes)
			//
		  // third entry is text file
		  'I' , 'N' , 'F' , 'O' , '_' , 'U' , 'F' , '2' , 'T' , 'X' , 'T' , 0x21, 0x00, 0xC6, 0x52, 0x6D,
		  0x65, 0x43, 0x65, 0x43, 0x00, 0x00, 0x88, 0x6D, 0x65, 0x43, 
		  0x03, 0x00, //cluster location
		  0x3E, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
	};

	//block size is not cluster size
	uint8_t DISK_data[2][DISK_BLOCK_SIZE] =
	{
	  {HTML_Page_Data},
	  {DATA_Page_Data}
	};
};


