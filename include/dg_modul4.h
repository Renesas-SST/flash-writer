/*
 * Copyright (c) 2015-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef RZV2H
#include <rzg2l_def.h>
#else
#include <rzv2h_def.h>
#endif
#include <stddef.h>

#define	CHANGE_OFF			0
#define	CHANGE_ON			1

#define	ADDITION			0x00000000
#define	SUBTRACTION			0x00000001

#if INTERNAL_MEMORY_ONLY
#define	LS_WORK_MEM_SADD		INTERNAL_WORK_MEMORY_SADD
#define	LS_WORK_MEM_EADD		INTERNAL_WORK_MEMORY_EADD
#define	WORK_SPI_LOAD_AREA		INTERNAL_WORK_SPI_LOAD_AREA
#else
#define	LS_WORK_MEM_SADD		DDR_WORK_MEMORY_SADD
#define	LS_WORK_MEM_EADD		DDR_WORK_MEMORY_EADD
#define	WORK_SPI_LOAD_AREA		DDR_WORK_SPI_LOAD_AREA
#endif

//Serial Flash ROM
#define	QSPI_BP_SIZE			0x0000200
#define	QSPI_BP_STARTAD			0x0000000

#define	SA_256KB			0x40000
#define	SA_64KB				0x10000
#define	SA_4KB				0x01000

#define	TOTAL_SIZE_256MB		0x10000000
#define	TOTAL_SIZE_128MB		0x08000000
#define	TOTAL_SIZE_64MB			0x04000000
#define	TOTAL_SIZE_32MB			0x02000000
#define	TOTAL_SIZE_16MB			0x01000000
#define	TOTAL_SIZE_8MB			0x00800000

#define	CYPRESS_MANUFACTURER_ID		0x01	/* Cypress	*/
#define	WINBOND_MANUFACTURER_ID		0xEF	/* Winbond	*/
#define	MACRONIX_MANUFACTURER_ID	0xC2	/* Macronix	*/
#define	MICRON_MANUFACTURER_ID		0x20	/* Micron	*/
#define	RENESAS_MANUFACTURER_ID		0x1F	/* Renesas	*/
#define	ISS_MANUFACTURER_ID		0x9D    /* ISS          */

#define	DEVICE_ID_IS25WP256		0x7019

#define	DEVICE_ID_S25FS128S		0x2018
#define	DEVICE_ID_S25FS512S		0x0220

#define	DEVICE_ID_W25Q64JV		0x4017
#define	DEVICE_ID_W25Q64JW		0x6017
#define	DEVICE_ID_W25Q128JV		0x4018
#define	DEVICE_ID_W25Q128JW		0x6018
#define	DEVICE_ID_W25Q256		0x4019
#define	DEVICE_ID_W25M512JV		0x7119
#define	DEVICE_ID_W25M512JW		0x6119
#define	DEVICE_ID_W25Q512JV		0x4020
#define	DEVICE_ID_W25Q512JV_DTR		0x7020

#define	DEVICE_ID_MX25L12805		0x2018
#define	DEVICE_ID_MX25L25645G		0x2019
#define	DEVICE_ID_MX25L51245G		0x201A
#define	DEVICE_ID_MX25U12835E		0x2538
#define	DEVICE_ID_MX66U25635F		0x2539
#define	DEVICE_ID_MX66U51235F		0x253A
#define	DEVICE_ID_MX66UM1G45G		0x803B
#define	DEVICE_ID_MX66UW1G45G		0x813B

#define	DEVICE_ID_MT25QL128		0xBA18
#define	DEVICE_ID_MT25QU128		0xBB18
#define	DEVICE_ID_MT25QL256		0xBA19
#define	DEVICE_ID_MT25QU256		0xBB19
#define	DEVICE_ID_MT25QL512		0xBA20
#define	DEVICE_ID_MT25QU512		0xBB20
#define	DEVICE_ID_MT25QL01G		0xBA21
#define	DEVICE_ID_MT25QU01G		0xBB21
#define	DEVICE_ID_MT25QL02G		0xBA22
#define	DEVICE_ID_MT25QU02G		0xBB22

#define	DEVICE_ID_AT25QL128A		0x4218
#define	DEVICE_ID_AT25SF128A		0x8901

typedef struct {
	uint16_t deviceId;
	const char* name;
	uint32_t sectorSize;
	uint32_t totalSize;
} FlashDeviceConfig;

// Represents a manufacturer and their supported devices
typedef struct {
	uint8_t manufacturerId;
	const char* name;
	const FlashDeviceConfig* devices;
	size_t deviceCount;
} Manufacturer;

typedef struct
{
	uint16_t deviceId;
	uint8_t manufacturerId;
	uint32_t sectorSize;
	uint32_t endAddress;
} FlashDevice;

// Load format enumeration for clarity
typedef enum {
	LOAD_SREC,
	LOAD_BINARY
} LoadFormat;

void dgG2InfoSpiflash0_BP(void);
void dgG2InfoSpiflash0_BP_S(void);
void dgClearSpiflash0(void);
void dgLoadSpiFlashSrec(void);
void dgLoadSpiFlashBinary(void);
void dgDisplayQspiData(void);
