/*
 * Copyright (c) 2023, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <mmio.h>
#include <sys.h>
#include <sys_regs.h>
#include <debug.h>
#include <stdbool.h>

#define	SYS_LSI_DEVID	(0x304)
#define	DEVID_MASK	(0x0FFFFFFFul)
#define	INFO_MASK	(0xF0000000ul)

void panic(void)
{
	while(1);
}

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

bool sys_is_resume_reboot(void)
{
	return false;
}
