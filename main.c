/*
 * Copyright (c) 2015-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include "main.h"
#include "dgtable.h"
#include "bit.h"
#include "timer.h"
#if EMMC == 1
#include "dg_emmc.h"
#endif /* EMMC == 1 */
#include "io.h"
#include "dg_modul1.h"
#include "dg_modul4.h"
#include "tzc_400.h"
#include "cpg.h"
#include "pfc.h"
#include "ddr.h"
#include "syc.h"
#include "sys.h"
#include "init_scif.h"
#include "scifdrv.h"
#include "memory_cmd.h"
#include "ddrcheck.h"
#include "ddr.h"
#include "spiflash1drv.h"

/********************************************************/
/*        DEVICE INFO                                   */
/********************************************************/
#define	RZV2H_DEVID		(0x847A447)
#define	RZG2L_DEVID		(0x841C447)
#define	RZV2L_DEVID		(0x8447447)
#define	RZG2UL_DEVID	(0x8450447)

#define WRITER_VERSION	"V1.00"		/* Software Version */
#define WRITER_DATE	"Jul.20,2025"	/* Release date */

static const com_menu MonCom[COMMAND_UNIT] __attribute__((section(".rodata"))) = {
    /* Command      Function Pointer                Parameter */
    /*--------------------- Basic Commands ---------------------*/
    { "H",          dgHelp,                         0 },
#if SERIAL_FLASH == 1
    /*--------------------- SPI Flash Commands -----------------*/
    { "XCS",        dgClearSpiflash0,        	      0 },
    { "XLS2",       dgLoadSpiFlashSrec,      	  	  0 },
    { "XLS3",       dgLoadSpiFlashBinary,      	      0 },
    // { "XRS",        dgDisplayQspiData,             0 },
    // { "XINFO_BP",   dgG2InfoSpiflash0_BP,          0 },
    // { "XINFO_BP_S", dgG2InfoSpiflash0_BP_S,        0 },
#endif /* SERIAL_FLASH == 1 */
#if EMMC == 1
    /*--------------------- eMMC Commands ----------------------*/
    { "EM_DCID",    dg_emmc_disp_cid,             0 },
    { "EM_DCSD",    dg_emmc_disp_csd,             0 },
    { "EM_DECSD",   dg_emmc_disp_ext_csd,         0 },
    { "EM_SECSD",   dg_emmc_set_ext_csd,          0 },
    { "EM_W",       dg_emmc_write_mot,            0 },
    { "EM_WB",      dg_emmc_write_bin,            0 },
    { "EM_E",       dg_emmc_erase,                0 },
#endif /* EMMC == 1 */
    /*--------------------- System Commands --------------------*/
    { "SUP",        dgScifSpeedUp,                 0 },
    { "SUD",        dgScifSpeedDown,               0 },
    { "RESET",      dgReset,                       0 },
#if INTERNAL_MEMORY_ONLY == 0
    /*--------------------- Memory Test Commands ---------------*/
    { "DDRCK",      dgDdrTest,                     0 },
    { "RAMCK",      dgRamTest,                     0 },
#endif /* INTERNAL_MEMORY_ONLY == 0 */
    /*--------------------- Memory Operations ------------------*/
    { "D",          dgDump,                        0 },
    { "DM",         dgDumpMode,                    0 },
    { "M",          dgMemEdit_byte,                0 },
    { "MW",         dgMemEdit_word,                0 },
    { "ML",         dgMemEdit_long,                0 },
    { "MX",         dgMemEdit_longlong,            0 },
    { "F",          dgFill_byte,                   0 },
    { "FL",         dgFill_long,                   0 },
    { "FX",         dgFill_longlong,               0 },
    /*--------------------- Table Terminator -------------------*/
    { TBL_END,      0,                             0 }
};

static const char *const StartMessMonitorV2H[] __attribute__((section(".rodata"))) = {
				"Flash writer for RZ/V2H Series "
				WRITER_VERSION " " WRITER_DATE,
				0,
};

static const char *const StartMessMonitorG2[] __attribute__((section(".rodata"))) = {
				"Flash writer for RZ/G2 Series "
				WRITER_VERSION " " WRITER_DATE,
				MESSAGE_END,
};

static const char *const StartMessMonitorV2[] __attribute__((section(".rodata"))) = {
				"Flash writer for RZ/V2 Series "
				WRITER_VERSION " " WRITER_DATE,
				MESSAGE_END,
};

static const char *const StartMessMonitorA[] __attribute__((section(".rodata"))) = {
				"Flash writer for RZ/A Series "
				WRITER_VERSION " " WRITER_DATE,
				MESSAGE_END,
};

char gKeyBuf[64] = {0};
int32_t gComNo = 0;
uintptr_t gUDump[2] = {0,255};
uintptr_t gUMem[2] = {0,255};
uint32_t gDumpMode = 0;

static void InitMain(void)
{
#if EMMC == 1
	dg_init_emmc();
#endif /* EMMC == 1 */
}

void StartMess( void )
{
	unsigned int devceid;

	devceid = sys_get_device_id();

	switch(devceid)
	{
		case RZG2L_DEVID:
			PutMess(StartMessMonitorG2);
			PutStr(" Product Code : ", 0);
#if (RZG2L == 1)
			PutStr("RZ/G2L" ,1);
#endif
#if (RZG2LC == 1)
			PutStr("RZ/G2LC" ,1);
#endif
		break;
		case RZV2L_DEVID:
			PutMess(StartMessMonitorV2);
			PutStr(" Product Code : ", 0);
			PutStr("RZ/V2L" ,1);
		break;
		case RZG2UL_DEVID:
#if (RZA3UL == 1)
			PutMess(StartMessMonitorA);
			PutStr(" Product Code : ", 0);
			PutStr("RZ/A3UL" ,1);
#else
			PutMess(StartMessMonitorG2);
			PutStr(" Product Code : ", 0);
#if (DEVICE_TYPE == 1)
			PutStr("RZ/G2UL Type1" ,1);
#else
			PutStr("RZ/G2UL Type2" ,1);
#endif
#endif
		break;
	case RZV2H_DEVID:
		PutMess(StartMessMonitorV2H);
		PutStr(" Product Code : ", 0);
		PutStr("RZ/V2H" ,1);
		break;
	default:
		break;
	}
	PutStr(">", 0);
}

static long CmpCom(char *str)
{
	char *cmpStr, *tmpStr, err;
	gComNo = 0;
	while(TBL_END != MonCom[gComNo].comStr)
	{
		err=0;
		tmpStr = str;
		cmpStr = MonCom[gComNo].comStr;
		while(*tmpStr!=0)
		{
			if (*tmpStr == *cmpStr)
			{
				tmpStr++;
				cmpStr++;
			}
			else
			{
				err = 1;
				break;
			}
		}
		if ((!err) && (*cmpStr == 0))
		{
			return(0);
		}
		gComNo++;
	}
	return(1);
}

static void DecCom(void)
{
	char tmp[64], chCnt, chPtr;
	uint32_t rtn = 0;
	uint32_t res;
	chCnt = 1;

	while (rtn == 0)
	{
		if (rtn == 0)
		{
			rtn = SCIF_TerminalInputCheck(gKeyBuf);
		}
		if (0 < rtn)
		{
			if (gKeyBuf[0] == CR_CODE)
			{
				PutChar(LF_CODE);
				PutChar(CR_CODE);
				PutStr(">", 0);
				chCnt = 0;

			}
			else if (gKeyBuf[0] != LF_CODE)
			{
				PutChar(gKeyBuf[0]);
			}
		}
	}
	while(1)
	{
		GetStr(gKeyBuf+chCnt,&chCnt);
		chPtr = 0;
		GetStrBlk(gKeyBuf,tmp,&chPtr,0);
		if (chPtr != 1)
		{
			ChgLtl2Lrg(tmp);
			if (!CmpCom(tmp))
			{
				(MonCom[gComNo].comProg)();
			}
			else
			{
				PutStr("command not found",1);
			}
		}
		PutStr(">",0);
		chCnt = 0;
	}
}

void Main(void)
{
	uint32_t readDevId;

	init_tzc_400();

	cpg_early_setup();
	pfc_setup();
	cpg_setup();

	InitScif();

#if (INTERNAL_MEMORY_ONLY == 0)
	ddr_setup();
#endif

	gDumpMode	= SIZE_8BIT;
#if (SERIAL_FLASH == 1)
	Init_SPIFlash();
#endif
	InitMain();
	StartMess();
	DecCom();
}
