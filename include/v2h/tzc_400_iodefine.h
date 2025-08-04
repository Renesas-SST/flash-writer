/*
 * Copyright (c) 2020-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**********************************************************************************************************************
 * File Name    : tzc_400_iodefine.h
 * Version      : 1.0
 * Description  : iodefine header file.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.09.2020 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include "rzv2h_def.h"


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef TZC_400_IODEFINE_H
#define TZC_400_IODEFINE_H

#define _TZC400_R8_BASE		        RZV2H_TZC400_R8_BASE
#define _TZC400_A55_BASE		    RZV2H_TZC400_A55_BASE
#define _TZC400_PCIe_BASE		    RZV2H_TZC400_PCIe_BASE
#define _TZC400_M33_BASE		    RZV2H_TZC400_M33_BASE
#define _TZC400_xSPI_BASE		    RZV2H_TZC400_xSPI_BASE
#define _TZC400_DDR00_BASE		    RZV2H_TZC400_DDR00_BASE
#define _TZC400_DDR01_BASE		    RZV2H_TZC400_DDR01_BASE
#define _TZC400_DDR10_BASE		    RZV2H_TZC400_DDR10_BASE
#define _TZC400_DDR11_BASE		    RZV2H_TZC400_DDR11_BASE

/*** Registers for ACPU SRAM ***/
#define TZC_GATE_KEEP_R8     (*(volatile uint32_t *)( _TZC400_R8_BASE + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_R8    (*(volatile uint32_t *)( _TZC400_R8_BASE_ + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_R8      (*(volatile uint32_t *)( _TZC400_R8_BASE_ + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_R8    (*(volatile uint32_t *)( _TZC400_R8_BASE_ + 0x114ul ))  /* Region ID access register */

/*** Registers for MCPU SRAM ***/
#define TZC_GATE_KEEP_A55     (*(volatile uint32_t *)( _TZC400_A55_BASE_ + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_A55    (*(volatile uint32_t *)( _TZC400_A55_BASE_ + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_A55      (*(volatile uint32_t *)( _TZC400_A55_BASE_ + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_A55    (*(volatile uint32_t *)( _TZC400_A55_BASE_ + 0x114ul ))  /* Region ID access register */

/*** Registers for SPI Multi ***/
#define TZC_GATE_KEEP_PCIe       (*(volatile uint32_t *)( _TZC400_PCIe_BASE   + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_PCIe      (*(volatile uint32_t *)( _TZC400_PCIe_BASE   + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_PCIe        (*(volatile uint32_t *)( _TZC400_PCIe_BASE   + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_PCIe      (*(volatile uint32_t *)( _TZC400_PCIe_BASE   + 0x114ul ))  /* Region ID access register */

/*** Registers for TZCDDR ***/
#define TZC_GATE_KEEP_M33    (*(volatile uint32_t *)( _TZC400_M33_BASE   + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_M33   (*(volatile uint32_t *)( _TZC400_M33_BASE   + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_M33     (*(volatile uint32_t *)( _TZC400_M33_BASE   + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_M33   (*(volatile uint32_t *)( _TZC400_M33_BASE   + 0x114ul ))  /* Region ID access register */

/*** Registers for ACPU SRAM ***/
#define TZC_GATE_KEEP_xSPI     (*(volatile uint32_t *)( _TZC400_xSPI_BASE + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_xSPI    (*(volatile uint32_t *)( _TZC400_xSPI_BASE + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_xSPI      (*(volatile uint32_t *)( _TZC400_xSPI_BASE + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_xSPI    (*(volatile uint32_t *)( _TZC400_xSPI_BASE + 0x114ul ))  /* Region ID access register */

/*** Registers for MCPU SRAM ***/
#define TZC_GATE_KEEP_DDR00     (*(volatile uint32_t *)( _TZC400_DDR00_BASE + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_DDR00    (*(volatile uint32_t *)( _TZC400_DDR00_BASE + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_DDR00      (*(volatile uint32_t *)( _TZC400_DDR00_BASE + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_DDR00    (*(volatile uint32_t *)( _TZC400_DDR00_BASE + 0x114ul ))  /* Region ID access register */

/*** Registers for SPI Multi ***/
#define TZC_GATE_KEEP_DDR01     (*(volatile uint32_t *)( _TZC400_DDR01_BASE + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_DDR01    (*(volatile uint32_t *)( _TZC400_DDR01_BASE + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_DDR01      (*(volatile uint32_t *)( _TZC400_DDR01_BASE + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_DDR01    (*(volatile uint32_t *)( _TZC400_DDR01_BASE + 0x114ul ))  /* Region ID access register */

/*** Registers for TZCDDR ***/
#define TZC_GATE_KEEP_DDR10     (*(volatile uint32_t *)( _TZC400_DDR10_BASE + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_DDR10    (*(volatile uint32_t *)( _TZC400_DDR10_BASE + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_DDR10      (*(volatile uint32_t *)( _TZC400_DDR10_BASE + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_DDR10    (*(volatile uint32_t *)( _TZC400_DDR10_BASE + 0x114ul ))  /* Region ID access register */

/*** Registers for TZCDDR ***/
#define TZC_GATE_KEEP_DDR11     (*(volatile uint32_t *)( _TZC400_DDR11_BASE + 0x008ul ))  /* Gate Keeper register */
#define TZC_SPECUL_CTL_DDR11    (*(volatile uint32_t *)( _TZC400_DDR11_BASE + 0x00Cul ))  /* Speculation control register */
#define TZC_RGN_ATTR_DDR11      (*(volatile uint32_t *)( _TZC400_DDR11_BASE + 0x110ul ))  /* Region attributes register */
#define TZC_RGN_ID_ACC_DDR11    (*(volatile uint32_t *)( _TZC400_DDR11_BASE + 0x114ul ))  /* Region ID access register */

/*** Mask/Setting bit patter for registers ***/
#define TZC_GATE_KEEP_SET_BIT   (0x0000000Ful)      /* Setting bit pattern */
#define TZC_SPECUL_CTL_SET_BIT  (0x00000003ul)      /* Setting bit pattern */
#define TZC_RGN_ATTR_MSK        (0x0FFFFFFFul)      /* Mask bit pattern */
#define TZC_RGN_ID_ACC_SET_BIT  (0x000F000Ful)      /* Setting bit pattern */

#define TZC_GATE_KEEP_OPEN_ALLFLT       (0x0000000Ful)  /* Open all filter unit */
#define TZC_SPECUL_DIS_PREFETCH         (0x00000003ul)  /* Disable pre-fetch */
#define TZC_RGN_ATTR_EN_ALLFLT          (0x0000000Ful)  /* Enable all filter unit */
#define TZC_RGN_ID_ACC_EN_RDWR_NSAID0_1 (0x000F000Ful)  /* Enable NSAIDW<x>�F 0, 1, NSAIDR<x>�F 0, 1   */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* TZC_400_IODEFINE_H */
