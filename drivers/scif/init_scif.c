/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include "scifdrv.h"
#include "init_scif.h"

void InitScif(void)
{
#ifndef RZV2H
	PowerOnScif0();
#endif
	InitScif0_SCIFCLK(115200);
}
