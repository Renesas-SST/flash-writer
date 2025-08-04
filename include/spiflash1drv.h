/*
 * Copyright (c) 2015-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
void Init_SPIFlash(void);
void ReadQspiFlashID(uint32_t *readData);
void ChipEraseQspiFlash(void);
void writeToFlash(uint32_t prgSpiStartAdd, uint32_t workAddMin, uint32_t workAddMax);

