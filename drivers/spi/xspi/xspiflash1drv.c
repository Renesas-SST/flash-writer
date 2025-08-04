/*
 * Copyright (c) 2015-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include "bit.h"
#include "spiflash1drv.h"
#include "xspi.h"
#include "xspi_puccini_regs.h"
#include "dg_modul4.h"
#include <rzv2h_def.h>

#define	DEBUG (0)

void Init_SPIFlash()
{
	xspi_setup((const uint32_t)XSPI_BASE, XSPI_MODE_X1_SEC);
}

static void EraseQspiFlash(uint32_t EraseStatAddr, uint32_t EraseEndAddr)
{
#if (DEBUG_MSG)
	char		str[64];
#endif
	uint32_t	sectorAd;
	uint32_t	start_addr, end_addr;

	start_addr = EraseStatAddr & 0xFFFFF000;
	end_addr  = EraseEndAddr  & 0xFFFFF000;
#if (DEBUG_MSG)
	PutStr("## EraseQspiFlash", 1);

	PutStr("Stat address : 0x", 0);
	Data2HexAscii(start_addr, str, 4);
	PutStr(str, 1);
	PutStr("End address : 0x", 0);
	Data2HexAscii(end_addr, str, 4);
	PutStr(str, 1);
#endif
	xspi_erase((const uint32_t)XSPI_BASE, start_addr, end_addr - start_addr);
	PutStr("Erase Completed ",1);
}

static void SaveDataQspiFlash(uint32_t srcAdd, uint32_t svFlashAdd, uint32_t svSize)
{
#if (DEBUG_MSG)
	char		str[64];
#endif
	uint32_t	flashAdd;
	uint32_t	writeDataAdd;

#if (DEBUG_MSG)
	PutStr("## SaveDataQspiFlash", 1);

	PutStr("srcAdd : 0x", 0);
	Data2HexAscii(srcAdd, str, 4);
	PutStr(str, 1);
	PutStr("svFlashAdd : 0x", 0);
	Data2HexAscii(svFlashAdd, str, 4);
	PutStr(str, 1);
	PutStr("svSize : 0x", 0);
	Data2HexAscii(svSize, str, 4);
	PutStr(str, 1);
#endif

	writeDataAdd = srcAdd;
	for (flashAdd = svFlashAdd; flashAdd<(svFlashAdd+svSize); flashAdd += 256)
	{
		// 256byte:RPC Write Buffer size
		xspi_write((const uint32_t)XSPI_BASE, flashAdd, writeDataAdd, 256);
		writeDataAdd = writeDataAdd + 256;
	}
}

void ReadQspiFlashID(uint32_t *readData)
{
	*readData = xspi_read_identification((const uint32_t)XSPI_BASE);
}

void ChipEraseQspiFlash(void)
{
	xspi_chip_erase((const uint32_t)XSPI_BASE);
}


void writeToFlash(uint32_t prgSpiStartAdd, uint32_t workAddMin, uint32_t workAddMax) {
    char str[64];
    uint32_t prgSpiEndAdd = prgSpiStartAdd + (workAddMax - workAddMin) - 1;

    PutStr("Erase SPI Flash memory...", 1);
    EraseQspiFlash(prgSpiStartAdd, prgSpiEndAdd);
    PutStr("Write to SPI Flash memory.", 1);
    SaveDataQspiFlash(LS_WORK_MEM_SADD, prgSpiStartAdd, prgSpiEndAdd - prgSpiStartAdd);

    PutStr("======= Qspi Save Information =================", 1);
    PutStr("SpiFlashMemory Stat Address : H'", 0);
    Data2HexAscii(prgSpiStartAdd, str, 4);
    PutStr(str, 1);
    PutStr("SpiFlashMemory End Address : H'", 0);
    Data2HexAscii(prgSpiEndAdd, str, 4);
    PutStr(str, 1);
    PutStr("===========================================================", 1);
    PutStr("", 1);
}