#pragma once
#include "stdint.h"
#include "util.h"
#include <hardware/flash.h>
#include <hardware/sync.h>


class PicoFlash {
public:
	PicoFlash() = default;

	/**
	 * Thanks to https://kevinboone.me/picoflash.html
	 *
	 * Read a chunk of data starting at `addr` from memory-mapped flash.
	 * An addr of 0x00 refers to the very first byte of flash.
	 */
	static void Read(uint32_t addr, void* buffer, uint32_t bufsize) {
		memcpy(buffer, (char*)(XIP_BASE + addr), bufsize);
	}

	/**
	 * Erases a sector of data starting at `sec_addr` aligned to a sector. An
	 * sec_addr of 0x00 refers to the very first byte of flash. `sectors` is
	 * the number of sectors to be erased.
	 */
	static void Erase(uint32_t sec_addr, size_t sectors) {
		safe_print("--------ERASE START-------\n");
		safe_print("Erasing %d sectors at sector-aligned address 0x%X\n", sectors, sec_addr);

		uint32_t ints = save_and_disable_interrupts();
		flash_range_erase(sec_addr, FLASH_SECTOR_SIZE * sectors);
		restore_interrupts (ints);

		safe_print("---------ERASE END--------\n");
		safe_print("\n");
	}

	/**
	 * Program data starting at `page_addr` aligned to a page (256-bytes).
	 * This assumes bufsize is always a multiple of page size and that the
	 * sections have previously been erased.
	 */
	static void Program(uint32_t page_addr, uint8_t* buffer, uint32_t bufsize) {
		safe_print("--------WRITE START-------\n");
		safe_print("Programming %d bytes to page-aligned address 0x%X\n", bufsize, page_addr);

		uint32_t ints = save_and_disable_interrupts();
		flash_range_program(page_addr, (uint8_t *)buffer, bufsize);
		restore_interrupts (ints);

		safe_print("---------WRITE END--------\n");
		safe_print("\n");
	}

	/**
	 * Write data to any page-aligned address while also minimizing the
	 * amount of erase calls. bufsize must be a multiple of page size. Writes
	 * must not "spill" into the next sector.
	 */
	static void Modify(uint32_t page_addr, uint8_t* buffer, uint32_t bufsize) {
		safe_print("--------MODIFY START-------\n");
		safe_print("Modifying %d bytes to page-aligned address 0x%X\n", bufsize, page_addr);
		size_t current_sector_num = page_addr / FLASH_SECTOR_SIZE;
		uint32_t sector_addr = current_sector_num * FLASH_SECTOR_SIZE;
		uint32_t sector_offset = page_addr - sector_addr;
		safe_print("0x%X is located in sector #%d of Pico, which starts at %X\n", page_addr, current_sector_num, sector_addr);
		safe_print("Compared to start of sector, modify is 0x%X bytes from the start of the sector\n", sector_offset);

		// Check if write is all within sector
		uint32_t sector_end_addr = sector_addr + FLASH_SECTOR_SIZE;
		if (bufsize > (sector_end_addr - page_addr)) {
			safe_print("Write will overexceed sector. Not writing.");
			safe_print("---------MODIFY END--------\n");
			return;
		}

		uint8_t sector_data[FLASH_SECTOR_SIZE];
		Read(sector_addr, sector_data, FLASH_SECTOR_SIZE);

		safe_print("Checking if modified data is already present\n");
		bool no_change = true;
		for(size_t i = sector_offset; i < sector_offset + bufsize; i++) {
			if (sector_data[i] != buffer[i - sector_offset]) {
				no_change = false;
				break;
			}
		}

		if (no_change) {
			safe_print("Data is already present. Not writing.\n");
			return;
		}

		safe_print("Data is not present.\n");
		safe_print("Checking if sector must be erased\n");

		bool is_erased = true;
		for(size_t i = sector_offset; i < sector_offset + bufsize; i++) {
			if (sector_data[i] != 0xFF) {
				is_erased = false;
				break;
			}
		}

		// Write Data 
		if (!is_erased) {
			safe_print("Sector is not erased. Erasing...\n");
			Erase(sector_addr, 1);

			safe_print("Modifying data already present...\n");
			for(size_t i = 0; i < bufsize; i++) {
				sector_data[sector_offset + i] = buffer[i];
			}

			safe_print("Rewriting sector with %d bytes at offset 0x%X changed...\n", bufsize, sector_offset);
			Program(sector_addr, sector_data, FLASH_SECTOR_SIZE);
			safe_print("Done");
		}
		else {
			safe_print("Sector is already erased. Writing %d bytes to 0x%X\n", bufsize, page_addr);
			Program(page_addr, buffer, bufsize);
		}

		safe_print("---------MODIFY END--------\n");
	}

};
