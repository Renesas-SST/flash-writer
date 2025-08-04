/*
 * Copyright (c) 2015-2023, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include "dgtable.h"
#include "dg_modul1.h"
#include "io.h"
#include "bit.h"
#include "timer.h"
#include "scifdrv.h"
#include "wdt.h"

static const char *const AllHelpMess[ALL_HELP_MESS_LINE] __attribute__((section(".rodata"))) = {
#if SERIAL_FLASH == 1
				"        SPI Flash write command",
				" XCS            erase program to SPI Flash",
				" XLS2           write program to SPI Flash",
				" XLS3           write program to SPI Flash(Binary)",
				"",
#endif /* SERIAL_FLASH == 1 */
#if EMMC == 1
				"        eMMC write command",
				" EM_DCID        display register CID",
				" EM_DCSD        display register CSD",
				" EM_DECSD       display register EXT_CSD",
				" EM_SECSD       change register EXT_CSD byte",
				" EM_W           write program to eMMC",
				" EM_WB          write program to eMMC (Binary)",
				" EM_E           erase program to eMMC",
				"",
#endif /* EMMC == 1 */
				" SUP            Scif speed UP (Change to speed up baud rate setting)",
				" SUD            Scif speed DOWN (Change to speed down baud rate setting)",
				" RESET          Perform RESET of the CPU",
				" H              help",
				" D  {sadr {eadr}}          memory dump  (DM sets dump size)",
				" DM {B|W|L|X}              set&disp dump mode",
				" M  [adr]                  set memory(BYTE)",
				" MW [adr]                  set memory(WORD)",
				" ML [adr]                  set memory(LONG)",
				" MX [adr]                  set memory(LONG LONG)",
				" F  [sadr] [eadr] [data]   fill memory",
				" FL [sadr] [eadr] [data]   fill memory(LONG)",
				" FX [sadr] [eadr] [data]   fill memory(LONG LONG)",
				MESSAGE_END,
};

extern char gKeyBuf[64];

uint32_t gFLASH_CS1_ID;

/************************************************
	MODULE			: dgHelp					*
	FUNCTION		: HELP	MESSAGE				*
	COMMAND			: H							*
	INPUT PARAMETER	: H							*
*************************************************/
void dgHelp(void)
{
	char tmp[64],chPtr,helpNo;

	chPtr=0;
	if(!GetStrBlk(gKeyBuf,tmp,&chPtr,0)){
		PutMess(AllHelpMess);
	}
}



int32_t	GetStr_ByteCount(char *str,uint32_t getByteCount)
{
	uint32_t	byteCount;
	int32_t i;

	for(byteCount=1;byteCount<=getByteCount;byteCount=byteCount+1){
		i=1;
		while(i==1){
			i=GetChar(str);
		}
		str++;
		i=1;
		while(i==1){
			i=GetChar(str);
		}
		str++;
	}
	*str = 0;
}

/********************************************************
	MODULE			: dgScifSpeedUp						*
	FUNCTION		: Scif speed UP	Change 921.6kbps	*
	COMMAND			: SUP								*
	INPUT PARAMETER	: SUP								*
*********************************************************/
void dgScifSpeedUp(void)
{
	PutStr("Scif speed UP",1);
	PutStr("Please change to 921.6Kbps baud rate setting of the terminal.",1);
	WaitPutCharSendEnd();
	udelay(1000);
	InitScif0_SCIFCLK(921600);
}

/************************************************************
	MODULE			: dgScifSpeedDown						*
	FUNCTION		: Scif speed Down	Change 115.2kbps	*
	COMMAND			: SDP									*
	INPUT PARAMETER	: SDP									*
*************************************************************/
void dgScifSpeedDown(void)
{
	PutStr("Scif speed DOWN",1);
	PutStr("Please change to 115.2Kbps baud rate setting of the terminal.",1);
	WaitPutCharSendEnd();
	udelay(1000);
	InitScif0_SCIFCLK(115200);
}

void dgReset(void)
{
	wdt_reset();
}
