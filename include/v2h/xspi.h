/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _XSPI_H_
#define _XSPI_H_

#include "xspi_puccini_regs.h"

#define XSPI_SUCCESS				(0)
#define XSPI_ERROR					(-1)

#define XSPI_WRITE_SIZE_SHIFT		(12)	/* 4k bytes */
#define XSPI_WRITE_PROG_SIZE		(1 << XSPI_WRITE_SIZE_SHIFT) /* 2^N bytes */

int32_t xspi_setup(const uint32_t xspi_base, const xspi_modes_t xspi_mode);
int xspi_erase(const uint32_t xspi_base, const uintptr_t addr, uint32_t byte_count);
int xspi_write(const uint32_t xspi_base, const uintptr_t addr, uintptr_t data, uint32_t byte_count);
uint32_t xspi_read_identification(const uint32_t xspi_base);
int xspi_chip_erase(const uint32_t xspi_base);

#endif	/* _XSPI_H_ */
