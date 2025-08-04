/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mmio.h>
#include "rzg2l_def.h"

#define SYS_BASE	RZG2L_SYS_BASE

#define	SYS_LSI_DEVID	(0xA04)
#define	DEVID_MASK	(0x0FFFFFFFul)
#define	INFO_MASK	(0xF0000000ul)

static inline uint32_t sys_reg_read(uint32_t offset)
{
	return mmio_read_32(SYS_BASE + offset);
}

unsigned int sys_get_device_id(void)
{
	return sys_reg_read(SYS_LSI_DEVID) & DEVID_MASK;
}

unsigned int sys_get_device_info(void)
{
	return ((sys_reg_read(SYS_LSI_DEVID) & INFO_MASK) >> 28) + 1;
}
