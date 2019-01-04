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

#include <string.h>
#include "pseudo_fat.h"

/* --- Boot Sector and BPB Structure --------------------------------------- */

/* #define USE_DUMMY_CODE */

static const uint8_t BootSector[] = {
    0xEB, 0x3C, 0x90,                                       /*00-02 - BS_jmpBoot */
    'm', 'k', 'f', 's', '.', 'f', 'a', 't',                 /*03-10 - BS_OEMName */
    htole16(BYTES_PER_SECTOR),                              /*11-12 - BPB_BytesPerSec */
    SECTORS_PER_CLUSTER,                                    /*13    - BPB_Sec_PerClus */
    htole16(RESERVED_SECTORS),                              /*14-15 - BPB_RsvdSecCnt */
    NUMBER_OF_FATS,                                         /*16    - BPB_NumFATs */
    htole16(ROOT_ENTRY_COUNT),                              /*17-18 - BPB_RootEntCnt */
    htole16(TOTAL_SECTORS),                                 /*19-20 - BPB_TotSec16 */
    FIXED_DISK,                                             /*21    - BPB_Media */
    htole16(FAT_SIZE),                                      /*22-23 - BPBFATSz16 */
    htole16(SECTORS_PER_TRACK),                             /*24-25 - BPB_SecPerTrk */
    htole16(NUMBER_OF_HEADS),                               /*26-27 - BPB_NumHeads */
    htole32(HIDDEN_SECTORS),                                /*28-31 - BPB_HiddSec */
    htole32(0),                                             /*32-35 - BPB_TotSec32 */
    HARD_DISK,                                              /*36    - BS_DrvNum */
    0,                                                      /*37    - BS_Reserved1 */
    BOOT_SIGNATURE,                                         /*38    - BS_BootSig */
    htole32(0x02DDA5BD),                                    /*39-42 - BS_VolID */
    'B', 'L', 'U', 'E', 'P', 'I', 'L', 'L', ' ', ' ', ' ',  /*43-53 - BS_VolLab */
    'F', 'A', 'T', '1', '2', ' ', ' ', ' ',                 /*54-61 - BS_FilSysType */

#ifdef USE_DUMMY_CODE
    0x0E, 0x1F, 0xBE, 0x5B, 0x7C, 0xAC, 0x22, 0xC0,         /*62-90 - Boot Code */
    0x74, 0x0B, 0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00,
    0xCD, 0x10, 0x5E, 0xEB, 0xF0, 0x32, 0xE4, 0xCD,
    0x16, 0xCD, 0x19, 0xEB, 0xFE,
    "This is not a bootable disk.  "                        /*91-192 - Boot Message */
    "Please insert a bootable floppy and\r\n"
    "press any key to try again ... \r\n",

#endif

/*   0x55, 0xAA                                               510-511 - Signature */
};

/* --- FAT12 Sector Structure ---------------------------------------------- */

#if FLASH_SIZE == (64 * 1024)

static const uint8_t FatSector[] = {
                      /* Dual Ent,   1st,   2nd, cl1, cl2 */
    0xF8, 0xFF, 0xFF, /* 0xFFFFF8, 0xFF8, 0xFFF, EOF, EOC */
    0x00, 0x40, 0x00, /* 0x004000, 0x000, 0x004,   0,   4 */
    0x05, 0x60, 0x00, /* 0x006005, 0x005, 0x006,   5,   6 */
    0x07, 0x80, 0x00, /* 0x008007, 0x007, 0x008,   7,   8 */
    0x09, 0xA0, 0x00, /* 0x00A009, 0x009, 0x00A,   9,  10 */
    0x0B, 0xC0, 0x00, /* 0x00C00B, 0x00B, 0x00C,  11,  12 */
    0x0D, 0xE0, 0x00, /* 0x00E00D, 0x00D, 0x00E,  13,  14 */
    0x0F, 0x00, 0x01, /* 0x01000F, 0x00F, 0x010,  15,  16 */
    0x11, 0x20, 0x01, /* 0x012011, 0x011, 0x012,  17,  18 */
    0x13, 0x40, 0x01, /* 0x014013, 0x013, 0x014,  19,  20 */
    0x15, 0x60, 0x01, /* 0x016015, 0x015, 0x016,  21,  22 */
    0x17, 0x80, 0x01, /* 0x018017, 0x017, 0x018,  23,  24 */
    0x19, 0xA0, 0x01, /* 0x01A019, 0x019, 0x01A,  25,  26 */
    0x1B, 0xC0, 0x01, /* 0x01C01B, 0x01B, 0x01C,  27,  28 */
    0x1D, 0xE0, 0x01, /* 0x01E01D, 0x01D, 0x01E,  29,  30 */
    0xFF, 0x0F,       /*    0xFFF, 0xFFF,         EOC     */
};

#elif FLASH_SIZE == (128 * 1024)

static const uint8_t FatSector[] = {
                      /* Dual Ent,   1st,   2nd, cl1, cl2 */
    0xF8, 0xFF, 0xFF, /* 0xFFFFF8, 0xFF8, 0xFFF, EOF, EOC */
    0x00, 0x40, 0x00, /* 0x004000, 0x000, 0x004,   0,   4 */
    0x05, 0x60, 0x00, /* 0x006005, 0x005, 0x006,   5,   6 */
    0x07, 0x80, 0x00, /* 0x008007, 0x007, 0x008,   7,   8 */
    0x09, 0xA0, 0x00, /* 0x00A009, 0x009, 0x00A,   9,  10 */
    0x0B, 0xC0, 0x00, /* 0x00C00B, 0x00B, 0x00C,  11,  12 */
    0x0D, 0xE0, 0x00, /* 0x00E00D, 0x00D, 0x00E,  13,  14 */
    0x0F, 0x00, 0x01, /* 0x01000F, 0x00F, 0x010,  15,  16 */
    0x11, 0x20, 0x01, /* 0x012011, 0x011, 0x012,  17,  18 */
    0x13, 0x40, 0x01, /* 0x014013, 0x013, 0x014,  19,  20 */
    0x15, 0x60, 0x01, /* 0x016015, 0x015, 0x016,  21,  22 */
    0x17, 0x80, 0x01, /* 0x018017, 0x017, 0x018,  23,  24 */
    0x19, 0xA0, 0x01, /* 0x01A019, 0x019, 0x01A,  25,  26 */
    0x1B, 0xC0, 0x01, /* 0x01C01B, 0x01B, 0x01C,  27,  28 */
    0x1D, 0xE0, 0x01, /* 0x01E01D, 0x01D, 0x01E,  29,  30 */
    0x1F, 0x00, 0x02, /* 0x02001F, 0x01F, 0x020,  31,  32 */
    0x21, 0x20, 0x02, /* 0x022021, 0x021, 0x022,  33,  34 */
    0x23, 0x40, 0x02, /* 0x024023, 0x023, 0x024,  35,  36 */
    0x25, 0x60, 0x02, /* 0x026025, 0x025, 0x026,  37,  38 */
    0x27, 0x80, 0x02, /* 0x028027, 0x027, 0x028,  39,  40 */
    0x29, 0xA0, 0x02, /* 0x02A029, 0x029, 0x02A,  41,  42 */
    0x2B, 0xC0, 0x02, /* 0x02C02B, 0x02B, 0x02C,  43,  44 */
    0x2D, 0xE0, 0x02, /* 0x02E02D, 0x02D, 0x02E,  45,  46 */
    0x2F, 0x00, 0x03, /* 0x03002F, 0x02F, 0x030,  47,  48 */
    0x31, 0x20, 0x03, /* 0x032031, 0x031, 0x032,  49,  50 */
    0x33, 0x40, 0x03, /* 0x034033, 0x033, 0x034,  51,  52 */
    0x35, 0x60, 0x03, /* 0x036035, 0x035, 0x036,  53,  54 */
    0x37, 0x80, 0x03, /* 0x038037, 0x037, 0x038,  55,  56 */
    0x39, 0xA0, 0x03, /* 0x03A039, 0x039, 0x03A,  57,  58 */
    0x3B, 0xC0, 0x03, /* 0x03C03B, 0x03B, 0x03C,  59,  60 */
    0x3D, 0xE0, 0x03, /* 0x03E03D, 0x03D, 0x03E,  61,  62 */
    0xFF, 0x0F,       /*    0xFFF, 0xFFF,        EOC      */
};

#endif

/* --- FAT 32 Byte Directory Entry Structure ------------------------------- */

static const uint8_t DirSector[] = {

    /* The firmware pseudo-file */
    'F', 'I', 'R', 'M', 'W', 'A', 'R', 'E', 'T', 'X', 'T',  /*00-10 - DIR_Name */
    ATTR_ARCHIVE,                                           /*11    - DIR_Attr */
    0,                                                      /*12    - DIR_NTRes */
    0,                                                      /*13    - DIR_CrtTimeTenth */
    FAT_TIME(17, 11, 32),                                   /*14-15 - DIR_CrtTime */
    FAT_DATE(25, 12, 2018),                                 /*16-17 - DIR_CrtDate */
    FAT_DATE(25, 12, 2018),                                 /*18-19 - DIR_LstAccDate */
    htole16(0),                                             /*20-21 - DIR_FstClusHI */
    FAT_TIME(17, 11, 32),                                   /*22-23 - DIR_WrtTime */
    FAT_DATE(25, 12, 2018),                                 /*24-25 - DIR_WrtDate */
    htole16(FIRST_CLUSTER),                                 /*26-27 - DIR_FstClusLO */
    htole16(RAM_DATA_SIZE),                                 /*28-31 - DIR_FileSize */

#ifdef USE_VOLUME_ID

    /* The volume ID, order is not important */
    'B', 'L', 'U', 'E', 'P', 'I', 'L', 'L', ' ', ' ', ' ',  /*32-42 - DIR_Name */
    ATTR_VOLUME_ID,                                         /*43    - DIR_Attr */
    0,                                                      /*44    - DIR_NTRes */
    0,                                                      /*45    - DIR_CrtTimeTenth */
    FAT_TIME(17, 11, 32),                                   /*46-47 - DIR_CrtTime */
    FAT_DATE(25, 12, 2018),                                 /*48-49 - DIR_CrtDate */
    FAT_DATE(25, 12, 2018),                                 /*50-51 - DIR_LstAccDate */
    htole16(0),                                             /*52-53 - DIR_FstClusHI */
    FAT_TIME(17, 11, 32),                                   /*54-55 - DIR_WrtTime */
    FAT_DATE(25, 12, 2018),                                 /*56-57 - DIR_WrtDate */
    htole16(0),                                             /*58-59 - DIR_FstClusLO */
    htole16(0),                                             /*60-63 - DIR_FileSize */

#endif

};

static uint8_t ram_data[RAM_DATA_SIZE];

#define FILL_RAM_DATA

int pseudo_fat_init(void)
{

#ifdef FILL_RAM_DATA

    int i;
    const uint8_t text[] = "USB Mass Storage Class example. ";

    /* Fill RAM data */
    for (i = 0; i < RAM_DATA_SIZE; i++) {
        ram_data[i] = text[i % (sizeof (text) - 1)];
    }

#endif

    return 0;
}

int pseudo_fat_read(uint32_t lba, uint8_t *sector)
{
    const uint8_t *buffer;
    size_t length;

    memset(sector, 0, BYTES_PER_SECTOR);
    switch (lba) {
    case 0:

        /* Sector 0 is the boot sector */
        buffer = BootSector;
	length = sizeof (BootSector);

	/* Add the boot sector signature (note that this is an
	 * absolute position in the boot sector), not relative to the
	 * end of the boot sector).
	 */
	sector[510] = 0x55;
	sector[511] = 0xAA;
	break;

    case 1:

        /* Sector 1 is the first FAT copy */

    case 2:

        /* Sector 2 is the second FAT copy */
        buffer = FatSector;
	length = sizeof (FatSector);
	break;

    case 3:

        /* Sector 3 is the directory entry */
        buffer = DirSector;
	length = sizeof (DirSector);
	break;

    default:

        /* Ignore reads outside of the data section */
        if (lba >= FILEDATA_START_SECTOR && lba < FILEDATA_END_SECTOR) {
	    buffer = ram_data + (lba - FILEDATA_START_SECTOR) * BYTES_PER_SECTOR;
	    length = BYTES_PER_SECTOR;
	} else {
	    return 0;
	}
	break;
    }
    memcpy(sector, buffer, length);
    return 0;
}

int pseudo_fat_write(uint32_t lba, const uint8_t *sector)
{

    /* Ignore writes outside of the data section */
    if (lba >= FILEDATA_START_SECTOR && lba < FILEDATA_END_SECTOR) {
        memcpy(ram_data + (lba - FILEDATA_START_SECTOR) * BYTES_PER_SECTOR,
	       sector, BYTES_PER_SECTOR);
    }
    return 0;
}
