/*
 * Copyright (c) 2021-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sys.h"
#include <ddr_internal.h>

void ddr_ctrl_reten_en_n(uint8_t val)
{
	if (sys_get_device_info() > 1)
	{
		val &= 1;
		write_phy_reg(DDRPHY_R79, (val << 1));
	}
	else
	{
		val &= 1;
		rmw_phy_reg(DDRPHY_R78, 0xFFFEFFFF, (val << 16));
	}
}
