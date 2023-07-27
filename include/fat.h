#pragma once
#include "stdint.h"
#include "fat_standard.hpp"
#include <hardware/flash.h>


class Fat16 {
public:
	enum CONFIG {
		DISK_BLOCK_NUM  = 0x3ffff,
		DISK_BLOCK_SIZE = 512,
		DISK_CLUSTER_SIZE = 8
	};

	// The computer you connect the Pico to thinks the Pico is a USB, thanks
	// to the TinyUSB library. But the Pico isn't actually a USB, it's just
	// acting as one. When the computer wants to read block 0x42A, that
	// block isn't an actual hexidecimal address on the Pico, it is a
	// "logical" block in a imaginary perfect file system; We can agree that
	// block 0x42 could be the start of (DATA, Root Dir, etc.), but the Pico
	// could store it however it likes. This enumeration is used to help
	// translate those LBA values into data stored on the executable and the
	// flash.
	static constexpr uint32_t INDEX_RESERVED = 0;
	static constexpr uint32_t INDEX_FAT_TABLE_1_START = 1; // Fat table size is 0x81
	static constexpr uint32_t INDEX_FAT_TABLE_2_START = 0x82;
	static constexpr uint32_t INDEX_ROOT_DIRECTORY = 0x103;
	static constexpr uint32_t INDEX_DATA_STARTS = 0x123;

	// Thanks to
	// https://www.makermatrix.com/blog/read-and-write-data-with-the-pi-pico-onboard-flash/
	//
	// Location on Pico flash memory, in bytes. This program is located in
	// the front of flash, meaning we must store FAT, Root Dir, and DATA
	// near the back.
	//
	// Useful information:
	// - Sector size is 4kb on Pico flash
	// - Due to how Flash works, you can't change a 0 to a 1. You'd need to
	// erase the whole sector, which sets everything to 1.
	static constexpr uint32_t FLASH_REFERENCE = (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE * 200);
	static constexpr uint32_t FLASH_FAT = FLASH_REFERENCE;
	static constexpr uint32_t FLASH_ROOT_DIRECTORY = FLASH_FAT + FLASH_SECTOR_SIZE;
	static constexpr uint32_t FLASH_DATA_START = FLASH_ROOT_DIRECTORY + 512 * 32;

public:
	Fat16();

	constexpr int GetBlockCount() const {
		return DISK_BLOCK_NUM;
	}

	constexpr int GetBlockSize() const {
		return DISK_BLOCK_SIZE;
	}

	int32_t GetBlock(const uint32_t lba, void* buffer, uint32_t bufsize);

	int32_t WriteBlock(const uint32_t lba, void* buffer, uint32_t bufsize);

	constexpr uint32_t LBAToIndex(const uint32_t lba) const;

	constexpr uint32_t LBAToFlash(const uint32_t lba) const;

private:
	fat::BootSector boot;
};


