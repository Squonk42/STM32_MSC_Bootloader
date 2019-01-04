/*
 * This file is part of the STM32 MSC Bootloader project.
 *
 * Copyright (C) 2018 Michel Stempin <michel.stempin@wanadoo.fr>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PSEUDO_FAT_H
#define __PSEUDO_FAT_H

#include <stdint.h>

#define FLASH_SIZE              (64 * 1024)
/* #define FLASH_SIZE           (128 * 1024) */
#define MSC_BOOTLOADER_SIZE     (8 * 1024)
#define MSC_FIRMWARE_SIZE       (FLASH_SIZE - MSC_BOOTLOADER_SIZE)

/* Size of the RAM data */
#define RAM_DATA_SIZE           (16 * 1024)

/* --- FAT definitions ----------------------------------------------------- */

/* For more details, see "Microsoft Extensible Firmware Initiative FAT32 File
 * System Specification, FAT: General Overview of On-Disk Format".
 * Version 1.03, December 6, 2000 (updated March 30, 2011).
 * Microsoft Coproration
 *
 * http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/fatgen103.doc
 */

/* Number of bytes per sector */
#define BYTES_PER_SECTOR        512

/* Number of sectors per cluster */
#define SECTORS_PER_CLUSTER     4

/* Number of reserved sectors (including boot sector) */
#define RESERVED_SECTORS        1

/* Number of FATs  */
#define NUMBER_OF_FATS          2

/* Count of root directory entries */
#define ROOT_ENTRY_COUNT        512

/* Media is fixed disk */
#define FIXED_DISK              0xF8

/* Size of FAT in sectors */
#define FAT_SIZE                1

/* Sectors per track */
#define SECTORS_PER_TRACK       32

/* Number of heads */
#define NUMBER_OF_HEADS         64

/* Hidden sectors */
#define HIDDEN_SECTORS          0

/* Drive is a hard disk */
#define HARD_DISK               0x80

/* Boot signature */
#define BOOT_SIGNATURE          0x29

/* Size of directory entry in bytes */
#define DIR_ENTRY_SIZE          32

/* File data start cluster number */
#define FIRST_CLUSTER           3

/* Number of FAT sectors */
#define FAT_SECTORS             (NUMBER_OF_FATS * FAT_SIZE)

/* Number of root directory sectors, rounded up */
#define ROOT_DIR_SECTORS        (((ROOT_ENTRY_COUNT * DIR_ENTRY_SIZE) + \
                                  (BYTES_PER_SECTOR - 1)) / \
                                 BYTES_PER_SECTOR)

/* First data sector */
#define FIRST_DATA_SECTOR       (RESERVED_SECTORS + FAT_SECTORS + \
                                 ROOT_DIR_SECTORS)

/* Actual file data start sector */
#define FILEDATA_START_SECTOR   (FIRST_DATA_SECTOR + \
                                 (FIRST_CLUSTER - 2) * SECTORS_PER_CLUSTER)
/* File data sector count */
#define FILEDATA_SECTOR_COUNT   (RAM_DATA_SIZE / BYTES_PER_SECTOR)

/* File data end sector */
#define FILEDATA_END_SECTOR     (FIRST_DATA_SECTOR + FILEDATA_SECTOR_COUNT)

/* Total sectors */
#define TOTAL_SECTORS           (FILEDATA_START_SECTOR + \
				 (MSC_FIRMWARE_SIZE / BYTES_PER_SECTOR))

/* Directory entry attributes */
#define ATTR_READ_ONLY		0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID		0x08
#define ATTR_DIRECTORY		0x10
#define ATTR_ARCHIVE		0x20
#define ATTR_LONG_NAME		(ATTR_READ_ONLY | \
				 ATTR_HIDDEN | \
				 ATTR_SYSTEM | \
				 ATTR_VOLUME_ID)
#define ATTR_LONG_NAME_MASK	(ATTR_READ_ONLY | \
				 ATTR_HIDDEN | \
				 ATTR_SYSTEM | \
				 ATTR_VOLUME_ID | \
				 ATTR_DIRECTORY | \
				 ATTR_ARCHIVE)

/* Convert a host 16-bit word into a 2-byte little-endian array */
#define htole16(x) ((x) & 0xFF), (((x) >> 8) & 0xFF)

/* Convert a host 32-bit double word into a 4-byte little-endian array */
#define htole32(x) ((x) & 0xFF), (((x) >> 8) & 0xFF), \
    (((x) >> 16) & 0xFF), (((x) >> 24) & 0xFF)

/* Format a FAT date:
 *      Bits 0–4: Day of month, valid value range 1-31 inclusive.
 *      Bits 5–8: Month of year, 1 = January, valid value range 1–12 inclusive.
 *      Bits 9–15: Count of years from 1980, valid value range 0–127 inclusive
 *                 (1980–2107).
 */
#define FAT_DATE(dd, mm, yyyy)  htole16(((yyyy - 1980) << 9) | (mm << 5) | dd)

/* Format a FAT time:
 *      Bits 0–4: 2-second count, valid value range 0–29 inclusive (0 – 58
 *                seconds).
 *      Bits 5–10: Minutes, valid value range 0–59 inclusive.
 *      Bits 11–15: Hours, valid value range 0–23 inclusive.
 *
 * The valid time range is from Midnight 00:00:00 to 23:59:58.
 */
#define FAT_TIME(hh, mm, ss)    htole16((hh << 11) | (mm << 5) | (ss >> 1))

extern int pseudo_fat_init(void);
extern int pseudo_fat_read(uint32_t lba, uint8_t *copy_to);
extern int pseudo_fat_write(uint32_t lba, const uint8_t *copy_from);

#endif
