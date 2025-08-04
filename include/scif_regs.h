/*
 * Copyright (c) 2021-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SCIF_REGS_H__
#define __SCIF_REGS_H__

#ifndef RZV2H
#include <rzg2l_def.h>
#else
#include <rzv2h_def.h>
#endif

/* SCIF ch0 */
#define SCIF0_SMR		(SCIF_BASE + 0x0000U)
#define SCIF0_BRR		(SCIF_BASE + 0x0002U)
#define SCIF0_MDDR		(SCIF_BASE + 0x0002U)
#define SCIF0_SCR		(SCIF_BASE + 0x0004U)
#define SCIF0_FTDR		(SCIF_BASE + 0x0006U)
#define SCIF0_FSR		(SCIF_BASE + 0x0008U)
#define SCIF0_FRDR		(SCIF_BASE + 0x000AU)
#define SCIF0_FCR		(SCIF_BASE + 0x000CU)
#define SCIF0_FDR		(SCIF_BASE + 0x000EU)
#define SCIF0_SPTR		(SCIF_BASE + 0x0010U)
#define SCIF0_LSR		(SCIF_BASE + 0x0012U)
#define SCIF0_SEMR		(SCIF_BASE + 0x0014U)

#endif	/* __SCIF_REGS_H__ */
