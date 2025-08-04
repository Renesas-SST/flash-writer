/*
 * Copyright (c) 2020, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**********************************************************************************************************************
 * File Name    : tzc_400.c
 * Version      : 1.0
 * Description  : TZC-400 driver.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.09.2020 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if 0
#include "iodefine.h"
#include "gpio_iodefine.h"
#endif
#include "tzc_400_iodefine.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: init_tzc_400
 * Description  : Initialize TAC-400 for using xSPI and DDR
 * Arguments    : none.
 * Return Value : none.
 *********************************************************************************************************************/
void init_tzc_400(void)
{
    /*** Initialize SPI Multi ***/
    TZC_RGN_ATTR_xSPI   = TZC_RGN_ATTR_EN_ALLFLT;
    TZC_RGN_ID_ACC_xSPI = TZC_RGN_ID_ACC_EN_RDWR_NSAID0_1;
    TZC_GATE_KEEP_xSPI  = TZC_GATE_KEEP_OPEN_ALLFLT;
    TZC_SPECUL_CTL_xSPI = TZC_SPECUL_DIS_PREFETCH;

#if (INTERNAL_MEMORY_ONLY == 0)
    /*** Initialize DDR ***/
    TZC_RGN_ATTR_DDR00   = TZC_RGN_ATTR_EN_ALLFLT;
    TZC_RGN_ID_ACC_DDR00 = TZC_RGN_ID_ACC_EN_RDWR_NSAID0_1;
    TZC_GATE_KEEP_DDR00  = TZC_GATE_KEEP_OPEN_ALLFLT;
    TZC_SPECUL_CTL_DDR00 = TZC_SPECUL_DIS_PREFETCH;

    TZC_RGN_ATTR_DDR01   = TZC_RGN_ATTR_EN_ALLFLT;
    TZC_RGN_ID_ACC_DDR01 = TZC_RGN_ID_ACC_EN_RDWR_NSAID0_1;
    TZC_GATE_KEEP_DDR01  = TZC_GATE_KEEP_OPEN_ALLFLT;
    TZC_SPECUL_CTL_DDR01 = TZC_SPECUL_DIS_PREFETCH;

    TZC_RGN_ATTR_DDR10   = TZC_RGN_ATTR_EN_ALLFLT;
    TZC_RGN_ID_ACC_DDR10 = TZC_RGN_ID_ACC_EN_RDWR_NSAID0_1;
    TZC_GATE_KEEP_DDR10  = TZC_GATE_KEEP_OPEN_ALLFLT;
    TZC_SPECUL_CTL_DDR10 = TZC_SPECUL_DIS_PREFETCH;

    TZC_RGN_ATTR_DDR11   = TZC_RGN_ATTR_EN_ALLFLT;
    TZC_RGN_ID_ACC_DDR11 = TZC_RGN_ID_ACC_EN_RDWR_NSAID0_1;
    TZC_GATE_KEEP_DDR11  = TZC_GATE_KEEP_OPEN_ALLFLT;
    TZC_SPECUL_CTL_DDR11 = TZC_SPECUL_DIS_PREFETCH;
#endif /* INTERNAL_MEMORY_ONLY = 0 */
}
/**********************************************************************************************************************
 * End of function init_tzc_400
 *********************************************************************************************************************/

