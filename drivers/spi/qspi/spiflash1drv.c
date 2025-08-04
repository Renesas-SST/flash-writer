/*
 * Copyright (c) 2015-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include "bit.h"
#include "spiflash1drv.h"
#include "rpcqspidrv.h"
#include "dgtable.h"
#include "dg_modul4.h"
#include <string.h>

#define	DEBUG_MSG	(0)
#define SPIREG_CR3V 0x00800004			//Volatile Status and Configuration Registers (CR3V)

extern FlashDevice gFlashDevice;

//////////////////////////////////
// Qspi:Fast Read  (4FAST_READ 0Ch)
//////////////////////////////////
static void Fast4RdQspiFlash(uint32_t sourceSpiAdd, uint32_t destinationAdd, uint32_t byteCount)
{
	uint32_t sourceAdd;

#if (DEBUG_MSG)
	char	str[64];

	PutStr("## Fast4RdQspiFlash", 1);
#endif
	InitRPC_QspiFlash4FastReadExtMode();

	sourceAdd = SPI_IOADDRESS_TOP + sourceSpiAdd;
#if (DEBUG_MSG)
	PutStr("## Fast4RdQspiFlash End", 1);

	PutStr("byteCount : 0x", 0);
	Data2HexAscii(byteCount, str, 4);
	PutStr(str, 1);

	PutStr("destinationAdd : 0x", 0);
	Data2HexAscii(destinationAdd, str, 4);
	PutStr(str, 1);
	PutStr("sourceAdd : 0x", 0);
	Data2HexAscii(sourceAdd, str, 4);
	PutStr(str, 1);
#endif
	memcpy((void*)destinationAdd, (const void *)sourceAdd, byteCount);
}

//////////////////////////////////
// Qspi:Fast Read  (FAST_READ 0Bh)
//////////////////////////////////
static void FastRdQspiFlash(uint32_t sourceSpiAdd,uint32_t destinationAdd,uint32_t byteCount)
{
	uint32_t sourceAdd;

#if (DEBUG_MSG)
	PutStr("## FastRdQspiFlash", 1);
#endif
	InitRPC_QspiFlashFastReadExtMode();

	sourceAdd = SPI_IOADDRESS_TOP + sourceSpiAdd;
	memcpy((void*)destinationAdd, (const void *)sourceAdd, byteCount);
}

//////////////////////////////////////////
// Qspi:Sector Erase (64kB)	
//////////////////////////////////////////
static void SectorEraseQspiFlashInternal(uint32_t addr)
{
	uint32_t	status;

#if (DEBUG_MSG)
	PutStr("## SectorEraseQspiFlashInternal", 1);
#endif
	WriteCommandQspiFlash(0x00060000);	//WRITE ENABLE
	if (gFlashDevice.endAddress <= TOTAL_SIZE_16MB)
	{
#if (DEBUG_MSG)
		PutStr("## SectorEraseQspiFlash", 1);
#endif
		// Sector Erase (D8h)
		SectorEraseQspiFlash(addr);
	}
	else
	{
#if (DEBUG_MSG)
		PutStr("## SectorErase4QspiFlash", 1);
#endif
		// Sector Erase with 4-Byte Address (DCh)
		SectorErase4QspiFlash(addr);
	}
	while(1)
	{
		ReadStatusQspiFlash(&status);
		if (!(status & BIT0))
		{
			break;
		}
	}
}

//////////////////////////////////////////
// Qspi:Bulk Erase (All)
//////////////////////////////////////////
static int32_t BulkEraseQspiFlash(void)
{
	uint32_t	status;

#if (DEBUG_MSG)
	PutStr("## BulkEraseQspiFlash", 1);
#endif

	WriteCommandQspiFlash(0x00060000);	//WRITE ENABLE
	WriteCommandQspiFlash(0x00600000);	//Bulk Erase (BE 60h)
	while(1)
	{
		ReadStatusQspiFlash(&status);
		if (!(status & BIT0))
		{
			break;		//BIT0  1:Device Busy  0:Ready Device is in Standby
		}
	}
	return(NORMAL_END);
}

//////////////////////////////////////////
// Page Program (No Wait)
//////////////////////////////////////////
static void PageProgramWithBuffeQspiFlashNoWait(uint32_t addr, uint32_t source_addr)
{
	uint32_t status;
#if (DEBUG_MSG)
	char	str[64];

	PutStr("## PageProgramWithBuffeQspiFlashNoWait", 1);

	PutStr("addr : 0x", 0);
	Data2HexAscii(addr, str, 4);
	PutStr(str, 1);
	PutStr("source_addr : 0x", 0);
	Data2HexAscii(source_addr, str, 4);
	PutStr(str, 1);
#endif
	WriteCommandQspiFlash(0x00060000);	//WRITE ENABLE
	if (gFlashDevice.endAddress <= TOTAL_SIZE_16MB)
	{
#if (DEBUG_MSG)
		PutStr("## WriteDataPpWithBuffer", 1);
#endif
		//Page Program (PP:02h)  3-byte address
		WriteDataPpWithBufferQspiFlash(addr, source_addr);
	}
	else
	{
#if (DEBUG_MSG)
		PutStr("## WriteData4ppWithBufferQspiFlash", 1);
#endif
		// Page Program with 4-Byte Address (12h)
		WriteData4ppWithBufferQspiFlash(addr, source_addr);
	}
}

//////////////////////////////////////////
// Page Program
//////////////////////////////////////////
static void PageProgramWithBuffeQspiFlash(uint32_t addr, uint32_t source_addr)
{
	uint32_t status;
#if (DEBUG_MSG)
	char	str[64];

	PutStr("## PageProgramWithBuffeQspiFlash", 1);

	PutStr("addr : 0x", 0);
	Data2HexAscii(addr, str, 4);
	PutStr(str, 1);
	PutStr("source_addr : 0x", 0);
	Data2HexAscii(source_addr, str, 4);
	PutStr(str, 1);
#endif
	WriteCommandQspiFlash(0x00060000);	//WRITE ENABLE
	if (gFlashDevice.endAddress <= TOTAL_SIZE_16MB)
	{
#if (DEBUG_MSG)
		PutStr("## WriteDataPpWithBuffer", 1);
#endif
		//Page Program (PP:02h)  3-byte address
		WriteDataPpWithBufferQspiFlash(addr, source_addr);
	}
	else
	{
#if (DEBUG_MSG)
		PutStr("## WriteData4ppWithBufferQspiFlash", 1);
#endif
		// Page Program with 4-Byte Address (12h)
		WriteData4ppWithBufferQspiFlash(addr, source_addr);
	}
	while(1)
	{
		ReadStatusQspiFlash(&status);
		if (!(status & BIT0))
		{
			break;
		}
	}
}

///////////////////////////////////////////////
// Qspi:Parameter 4-kB Sector Erase
///////////////////////////////////////////////
static void ParameterSectorErase4kbQspiFlash(uint32_t addr)
{
	uint32_t	status;
#if (DEBUG_MSG)
	char		str[64];

	PutStr("## ParameterSectorErase4kbQspiFlash", 1);
	PutStr("addr : 0x", 0);
	Data2HexAscii(addr, str, 4);
	PutStr(str, 1);
#endif
	WriteCommandQspiFlash(0x00060000);	//WRITE ENABLE
	if (gFlashDevice.endAddress <= TOTAL_SIZE_16MB)
	{
		// 4KB Sector Erase (20h)
		ParameterSectorErase3QspiFlash(addr);
	}
	else
	{
		// 4KB Sector Erase with 4-Byte Address (21h)
		ParameterSectorErase4QspiFlash(addr);
	}
	while(1)
	{
		ReadStatusQspiFlash(&status);
		if (!(status & BIT0))
		{
			break;
		}
	}
}

///////////////////////////////////////////////////////////////
// SetSectorErase256kbQspiFlashCypress
///////////////////////////////////////////////////////////////
static void SetSectorErase256kbQspiFlashCypress(void)
{
	unsigned char	readData;
	uint32_t		status;
	uint32_t		addr;

	char str[64];

	addr = (uint32_t)SPIREG_CR3V;
	ReadAnyRegisterQspiFlash(addr, &readData);

	if (!(readData & BIT1))
	{
		WriteCommandQspiFlash(0x00060000);
		readData |= BIT1;			// Bit1=Block Erase Size  1:256KB , 0:64KB
		WriteAnyRegisterQspiFlash(addr, readData);
		while(1)
		{
			ReadStatusQspiFlash(&status);
			if (!(status & BIT0))
			{
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////
// SetSectorErase64kbQspiFlashCypress
///////////////////////////////////////////////////////////////
static void SetSectorErase64kbQspiFlashCypress(void)
{
	unsigned char	readData;
	uint32_t		status;
	uint32_t		addr;

	char str[64];

	addr = (uint32_t)SPIREG_CR3V;
	ReadAnyRegisterQspiFlash(addr, &readData);

	if (readData & BIT1)
	{
		WriteCommandQspiFlash(0x00060000);
		readData &= ~BIT1;			// Bit1=Block Erase Size  1:256KB , 0:64KB
		WriteAnyRegisterQspiFlash(addr, readData);
		while(1)
		{
			ReadStatusQspiFlash(&status);
			if (!(status & BIT0))
			{
				break;
			}
		}
	}
}

//////////////////////////////////////////
// SaveDataWithBuffeQspiFlash
//////////////////////////////////////////
static void SaveDataWithBuffeQspiFlash(uint32_t srcAdd, uint32_t svFlashAdd, uint32_t svSize)
{
#if (DEBUG_MSG)
	char		str[64];
#endif
	uint32_t	flashAdd;
	uint32_t	writeDataAdd;

#if (DEBUG_MSG)
	PutStr("## SaveDataWithBuffeQspiFlash", 1);

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
	WriteCommandQspiFlash(0x00060000);	//WRITE ENABLE

	writeDataAdd = srcAdd;
	for (flashAdd = svFlashAdd; flashAdd<(svFlashAdd+svSize); flashAdd += 256)
	{
		// 256byte:RPC Write Buffer size
		PageProgramWithBuffeQspiFlash(flashAdd, writeDataAdd);
		writeDataAdd = writeDataAdd + 256;
	}
}

//////////////////////////////////////////
// SectorEraseQspi_Flash
//////////////////////////////////////////
static void SectorEraseQspi_Flash(uint32_t EraseStatAdd, uint32_t EraseEndAdd)
{
#if (DEBUG_MSG)
	char		str[64];
#endif
	uint32_t	sectorAd;
	uint32_t	SectorStatTopAdd, SectorEndTopAdd;
	SectorStatTopAdd = EraseStatAdd & ((~gFlashDevice.sectorSize) + 1);
	SectorEndTopAdd  = EraseEndAdd  & ((~gFlashDevice.sectorSize) + 1);

#if (DEBUG_MSG)
	PutStr("## SectorEraseQspi_Flash", 1);

	PutStr("SectorStatTopAdd : 0x", 0);
	Data2HexAscii(SectorStatTopAdd, str, 4);
	PutStr(str, 1);
	PutStr("SectorEndTopAdd : 0x", 0);
	Data2HexAscii(SectorEndTopAdd, str, 4);
	PutStr(str, 1);
	PutStr("SectorSize : 0x", 0);
	Data2HexAscii(gFlashDevice.sectorSize, str, 4);
	PutStr(str, 1);
#endif

	if ((gFlashDevice.manufacturerId == CYPRESS_MANUFACTURER_ID ) && (gFlashDevice.deviceId == DEVICE_ID_S25FS128S))
	{
		if (gFlashDevice.sectorSize == SA_256KB)
		{
			PutStr("## 256KB Sector" , 1);
			SetSectorErase256kbQspiFlashCypress();
		}
		else
		{
			PutStr("## 64KB Sector" , 1);
			SetSectorErase64kbQspiFlashCypress();
		}
	}
	for (sectorAd = SectorStatTopAdd; sectorAd <= SectorEndTopAdd; sectorAd = sectorAd + gFlashDevice.sectorSize)
	{
		SectorEraseQspiFlashInternal(sectorAd);
		PutStr(".",0);
	}
	PutStr("Erase Completed ",1);
}

///////////////////////////////////////////////////////
// ParameterSectorEraseQspiFlash (4KB Sector Erase)
///////////////////////////////////////////////////////
static void ParameterSectorEraseQspiFlash(uint32_t EraseStatAdd,uint32_t EraseEndAdd)
{
#if (DEBUG_MSG)
	char		str[64];
#endif
	uint32_t	sectorAd;
	uint32_t	SectorStatTopAdd,SectorEndTopAdd;

	SectorStatTopAdd = EraseStatAdd & 0xFFFFF000;
	SectorEndTopAdd  = EraseEndAdd  & 0xFFFFF000;
#if (DEBUG_MSG)
	PutStr("## ParameterSectorEraseQspiFlash", 1);

	PutStr("SectorStatTopAdd : 0x", 0);
	Data2HexAscii(SectorStatTopAdd, str, 4);
	PutStr(str, 1);
	PutStr("SectorEndTopAdd : 0x", 0);
	Data2HexAscii(SectorEndTopAdd, str, 4);
	PutStr(str, 1);
#endif

	for (sectorAd = SectorStatTopAdd;sectorAd <= SectorEndTopAdd; sectorAd = sectorAd + 0x1000)
	{
		ParameterSectorErase4kbQspiFlash(sectorAd);
		PutStr(".", 0);
	}
	PutStr("Erase Completed ",1);
}

//////////////////////////////////////////
// SectorRdQspiFlash
//////////////////////////////////////////
static void SectorRdQspiFlash(uint32_t spiStatAdd, uint32_t distRamAdd)
{
#if (DEBUG_MSG)
	char		str[64];
#endif
	uint32_t	SectorStatTopAdd, readSize;

	SectorStatTopAdd = spiStatAdd & ((~gFlashDevice.sectorSize) + 1);
	readSize         = gFlashDevice.sectorSize;

#if (DEBUG_MSG)
	PutStr("## SectorRdQspiFlash", 1);

	PutStr("SectorStatTopAdd : 0x", 0);
	Data2HexAscii(SectorStatTopAdd, str, 4);
	PutStr(str, 1);
	PutStr("readSize : 0x", 0);
	Data2HexAscii(readSize, str, 4);
	PutStr(str, 1);
#endif
	if (gFlashDevice.endAddress <= TOTAL_SIZE_16MB)
	{
		FastRdQspiFlash(SectorStatTopAdd, distRamAdd, readSize);
	}
	else
	{
		Fast4RdQspiFlash(SectorStatTopAdd, distRamAdd, readSize);
	}
}

//////////////////////////////////////////
// CkSpiFlashAllF
//////////////////////////////////////////
static int32_t CkSpiFlashAllF(int32_t sAdd,int32_t cap)
{
	uintptr_t	ckAdd;
	unsigned char	rdData;

	for (ckAdd = sAdd; ckAdd < (sAdd+cap); ckAdd++)
	{
		rdData = *((volatile unsigned char*)ckAdd);
		if (rdData != 0xFF)
		{
			return(1);
		}
	}
	return(0);
}

//////////////////////////////////////////
// CkQspiFlash1ClearSectorSize
//////////////////////////////////////////
static bool CkQspiFlash1ClearSectorSize(uint32_t rdBufAdd, uint32_t spiFlashStartAdd, uint32_t checkSize, uint32_t orgSpiFlashStartAdd, uint32_t orgSpiEndAdd)
{
	uint32_t	flashStatus, flashEraseFlg;
	uint32_t	deltaS, deltaE;
	char 		str1Buf[10], str2Buf[10];
	char		str[64];

	deltaS = orgSpiFlashStartAdd - spiFlashStartAdd;;
	deltaE = (spiFlashStartAdd + checkSize - orgSpiEndAdd - 1) & 0xFFFFFF00U;

	PutStr("SPI Data Clear(H'FF) Check :",0);
	if (gFlashDevice.endAddress <= TOTAL_SIZE_16MB)
	{
		FastRdQspiFlash(spiFlashStartAdd, rdBufAdd, checkSize);
	}
	else
	{
		Fast4RdQspiFlash(spiFlashStartAdd, rdBufAdd, checkSize);
	}

	flashEraseFlg = 0;

	if (CkSpiFlashAllF(rdBufAdd, checkSize))
	{
		PutStr("H'",0);
		Data2HexAscii(spiFlashStartAdd, str1Buf, 4);
		PutStr(&str1Buf[0], 0);
		PutStr("-", 0);
		Data2HexAscii(((spiFlashStartAdd + checkSize) - 1), str2Buf, 4);
		PutStr(&str2Buf[0], 0);
		PutStr(",Clear OK?(y/n)", 0);

		if (WaitKeyIn_YorN())
		{
			DelStr(34);
			PutStr(" Exit ", 1);
			return false;
		}
		DelStr(34);
		flashEraseFlg = 1;
	}
	else
	{
		PutStr(" OK ",1);
	}
	if (flashEraseFlg)
	{
		PutStr("H'",0);
		PutStr(&str1Buf[0], 0);
		PutStr("-",0);
		PutStr(&str2Buf[0], 0);
		PutStr(" Erasing.", 0);
   		SectorEraseQspi_Flash(spiFlashStartAdd, ((spiFlashStartAdd + checkSize) - 1));

		if (deltaS)
		{
			SaveDataWithBuffeQspiFlash(rdBufAdd, spiFlashStartAdd, deltaS);	// ReStore
		}
		if (deltaE)
		{
			SaveDataWithBuffeQspiFlash((rdBufAdd + checkSize - deltaE), (spiFlashStartAdd + checkSize - deltaE), deltaE);	// ReStore
		}
	}
	return true;
}

void writeToFlash(uint32_t prgSpiStartAdd, uint32_t workAddMin, uint32_t workAddMax) {
	char str[64];
    uint32_t maskSectorSize = (~gFlashDevice.sectorSize) + 1;
    uint32_t prgSpiEndAdd = prgSpiStartAdd + (workAddMax - workAddMin);
    uint32_t saveSize = prgSpiEndAdd - prgSpiStartAdd + 1;
    uint32_t clrSpiStartSecTopAdd = prgSpiStartAdd & maskSectorSize;
    uint32_t clrSpiSecEndAdd = prgSpiEndAdd | ~maskSectorSize;
    uint32_t clearSize = clrSpiSecEndAdd - clrSpiStartSecTopAdd + 1;
    uint32_t readWorkStartAdd = WORK_SPI_LOAD_AREA;

    if (!CkQspiFlash1ClearSectorSize(readWorkStartAdd, clrSpiStartSecTopAdd, 
                                   clearSize, prgSpiStartAdd, prgSpiEndAdd)) {
        return;
    }

    PutStr("SAVE SPI-FLASH.......", 0);
    SaveDataWithBuffeQspiFlash(workAddMin, prgSpiStartAdd, saveSize);
    PutStr("", 1);
    PutStr("======= Qspi  Save Information  =================", 1);
    PutStr(" SpiFlashMemory Stat Address : H'", 0);
	Data2HexAscii(prgSpiStartAdd, str, 4);
	PutStr(str, 1);
    PutStr(" SpiFlashMemory End Address  : H'", 0);
	Data2HexAscii(prgSpiEndAdd, str, 4);
	PutStr(str, 1);
    PutStr("===========================================================", 1);
    PutStr("", 1);
}

void Init_SPIFlash(void)
{
	InitRPC_QspiFlash(RPC_CLK_40M);
}

void ChipEraseQspiFlash(void)
{
	BulkEraseQspiFlash();
}
