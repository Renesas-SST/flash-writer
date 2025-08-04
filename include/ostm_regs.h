/*
 * Copyright (c) 2021-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __OSTM_REGS_H__
#define __OSTM_REGS_H__

#ifndef RZV2H
#include <rzg2l_def.h>
#else
#include <rzv2h_def.h>
#endif

// OS Timer
#define OSTM0CMP		(OSTM0_BASE + 0x0000U)
#define OSTM0CNT		(OSTM0_BASE + 0x0004U)
#define OSTM0TE			(OSTM0_BASE + 0x0010U)
#define OSTM0TS			(OSTM0_BASE + 0x0014U)
#define OSTM0TT			(OSTM0_BASE + 0x0018U)
#define OSTM0CTL		(OSTM0_BASE + 0x0020U)

#endif	/* __OSTM_REGS_H__ */
