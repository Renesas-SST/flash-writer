/*
 * Copyright (c) 2023-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <mmio.h>
#include <rzv2h_def.h>
#include <xspi_puccini_regs.h>
#include <xspi.h>
#include <sys.h>
#include <debug.h>
#include <common.h>

extern void panic(void);

#define XSPI_IN						U(0)
#define XSPI_OUT					U(1)

#define RDID						(0)
#define RSTEN						(1)
#define RESET						(2)
#define WTEN						(3)
#define ERASE						(4)
#define RDSTA						(5)
#define WRITE						(6)
#define CHIP_ERASE 					(7)

#define DEVID_ID_MASK				UL(0x00FFFFFF)
#define DEVICE_ID_BAD				UL(0)
#define DEVICE_ID_ERROR				UL(0x00FFFFFF)

#define XSPI_COMMAND_TIMEOUT		U(100000)

#define XSPI_ADDR(x)				(xspi_base + (x))

#define XSPI_BASE_ADDRESS_OFFSET 	0x20000000
#define XSPI_STATUS_BUSY_MASK 		0x01

typedef struct {
	uint16_t instruction;
	uint8_t	 direction : 3;		/* Direction */
	uint8_t	 latency   : 5;		/* Latency (cycle) */
	uint8_t	 data_size : 4;		/* Data size (byte) */
	uint8_t	 addr_size : 4;		/* Address size (byte) */
	uint8_t	 inst_size : 4;		/* Instruction size (byte) */
} st_xspi_cmd_t;

typedef struct {
	uint8_t	 cmd_idx;
	uint32_t addr;
	uint32_t data;
} st_xspi_cmd_info_t;

static const st_xspi_cmd_t cmds[] = {
/* {instruction,   direction,	latency,	data_size,	addr_size,	cmd_size}*/
	{0x9F00u,		XSPI_IN,	0u,			3u,			0u,			1u},	/* RDID */
	{0x6600u,		XSPI_OUT,	0u,			0u,			0u,			1u},	/* RSTEN */
	{0x9900u,		XSPI_OUT,	0u,			0u,			0u,			1u},	/* RESET */
	{0x0600u,		XSPI_OUT,	0u,			0u,			0u,			1u},	/* WTEN */
	{0x2000u,		XSPI_OUT,	0u,			0u,			3u,			1u},	/* ERASE */
	{0x0500u,		XSPI_IN,	0u,			1u,			0u,			1u},	/* RDSTA */
	{0x0200u,		XSPI_OUT,	0u,			4u,			3u,			1u},	/* WRITE */
	{0x6000u,		XSPI_OUT,	0u,		    0u,		    0u,		    1u}     /* Chip Erase */
};


static int32_t xspi_single_command(const uint32_t xspi_base, const st_xspi_cmd_info_t * const p_cmd_info)
{
	uint32_t timeout;

	mmio_write_32(XSPI_ADDR(XSPI_CDCTL0_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_CDCTL0_OFFSET)) & (~XSPI_CDCTL0_TRREQ_MSK));

	mmio_write_32(XSPI_ADDR(XSPI_CDTBUF0_OFFSET), (((uint32_t)cmds[p_cmd_info->cmd_idx].instruction) << XSPI_CDTBUF_CMD_OFFSET)
												| (((uint32_t)cmds[p_cmd_info->cmd_idx].direction)	<< XSPI_CDTBUF_TRTYPE_OFFSET)
												| (((uint32_t)cmds[p_cmd_info->cmd_idx].latency)	<< XSPI_CDTBUF_LATE_OFFSET)
												| (((uint32_t)cmds[p_cmd_info->cmd_idx].data_size)	<< XSPI_CDTBUF_DATASIZE_OFFSET)
												| (((uint32_t)cmds[p_cmd_info->cmd_idx].addr_size)	<< XSPI_CDTBUF_ADDSIZE_OFFSET)
												| (((uint32_t)cmds[p_cmd_info->cmd_idx].inst_size)	<< XSPI_CDTBUF_CMDSIZE_OFFSET));

	mmio_write_32(XSPI_ADDR(XSPI_CDABUF0_OFFSET), p_cmd_info->addr);

	if (cmds[p_cmd_info->cmd_idx].direction == XSPI_OUT) {
		mmio_write_32(XSPI_ADDR(XSPI_CDD0BUF0_OFFSET), p_cmd_info->data);
	}

	mmio_write_32(XSPI_ADDR(XSPI_CDCTL0_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_CDCTL0_OFFSET)) | XSPI_CDCTL0_TRREQ_MSK);

	timeout = XSPI_COMMAND_TIMEOUT;
	/* Wait for command to complete */
	while ((0u == (mmio_read_32(XSPI_ADDR(XSPI_INTS_OFFSET)) & XSPI_INTS_CMDCMP_MSK)) && (timeout > 0u)) {
		timeout--;
		__asm__ ("nop");
		__asm__ ("dsb sy" : : : "memory");
	}

	mmio_write_32(XSPI_ADDR(XSPI_INTC_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_INTC_OFFSET)) | XSPI_INTC_CMDCMPC_MSK);

	return (timeout == 0u) ? XSPI_ERROR : XSPI_SUCCESS;
}

static int32_t xspi_reset(const uint32_t xspi_base, const xspi_modes_t xspi_mode)
{
	int ret;

	switch (xspi_mode) {
	case XSPI_MODE_X1_SEC: {
		/* Issue the reset command */
		st_xspi_cmd_info_t cmd_rsten = {RSTEN, 0, 0};
		st_xspi_cmd_info_t cmd_reset = {RESET, 0, 0};

		ret = xspi_single_command(xspi_base, &cmd_rsten);
		if (ret == XSPI_SUCCESS) {
			ret = xspi_single_command(xspi_base, &cmd_reset);
		}
	}
	break;

	case XSPI_MODE_X8_SEC: {
		uint32_t i = 0;

		/* Toggle the RESETCS0# pin. Start with clearing pin - Reset active */
		mmio_write_32(XSPI_ADDR(XSPI_LIOCTL_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_LIOCTL_OFFSET)) & (~XSPI_LIOCTL_RSTCS0_MSK));
		/* Make a delay before releasing the reset - reduced from 1000u to 100u as per recommendation from RTL */
		for (i = 0; i < 100u; i++) {
			__asm__ ("nop");
		}
		/* Release the reset */
		mmio_write_32(XSPI_ADDR(XSPI_LIOCTL_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_LIOCTL_OFFSET)) | XSPI_LIOCTL_RSTCS0_MSK);
		ret = XSPI_SUCCESS;
	}
	break;

	default: {
		/* Error */
		panic();
	}
	break;
	}

	return ret;
}

static void xspi_init_registers(const uint32_t xspi_base, const xspi_modes_t xspi_mode)
{
	switch (xspi_mode) {
	case XSPI_MODE_X1_SEC: {
		/* Set SPI mode - sample data at falling edge, drive clock at falling edge, 1S-1S-1S */
		mmio_write_32(XSPI_ADDR(XSPI_LIOCFGCS0_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_LIOCFGCS0_OFFSET)) | XSPI_LIOCFGCS_PRTMD_1S_1S_1S);
		/* Set memory mapping mode */
		mmio_write_32(XSPI_ADDR(XSPI_CMCFG0CS0_OFFSET), (0x02u << XSPI_CMCFG0CS_ADDSIZE));
		/* 0 latency, 0x0300 read command */
		mmio_write_32(XSPI_ADDR(XSPI_CMCFG1CS0_OFFSET), (0x00u << XSPI_CMCFG1CS_RDLATE) | (0x0300u << XSPI_CMCFG1CS_RDCMD));
		/* 0 latency, 0x0200 write command */
		mmio_write_32(XSPI_ADDR(XSPI_CMCFG2CS0_OFFSET), (0x00u << XSPI_CMCFG1CS_WRLATE) | (0x0200u << XSPI_CMCFG1CS_WRCMD));
		/* Enable read access to CS0 for selected channel memory areas */
		mmio_write_32(XSPI_ADDR(XSPI_BMCTL0_OFFSET), (0x01u << XSPI_BMCTL0_CS0ACC));
		/* Enable prefetch */
		mmio_write_32(XSPI_ADDR(XSPI_BMCFG_OFFSET), (0x01u << XSPI_BMCFG_PREEN) | (0x01u << XSPI_BMCFG_WRMD));

		/* WRAPCFG: use default values */
		/* COMCFG:	use default values */
		/* BMCFG:	use default values */
		/* CSSCTL:	use default values */
	}
	break;

	case XSPI_MODE_X8_SEC: {
		mmio_write_32(XSPI_ADDR(XSPI_WRAPCFG_OFFSET), (0x1Fu << XSPI_WRAPCFG_DSSFTCS0));

		/* Set SPI mode - 8D-8D-8D */
		mmio_write_32(XSPI_ADDR(XSPI_LIOCFGCS0_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_LIOCFGCS0_OFFSET))
														| (XSPI_LIOCFGCS_PRTMD_8D_8D_8D << XSPI_LIOCFGCS_PRTMD)
														| (0x01u << XSPI_LIOCFGCS_LATEMD));

		/* Set memory mapping mode */
		mmio_write_32(XSPI_ADDR(XSPI_CMCFG0CS0_OFFSET), (0x03u << XSPI_CMCFG0CS_ADDSIZE) | (0x02u << XSPI_CMCFG0CS_FFMT));
		/* 10 cycle latency, 0xA000 read command */
		mmio_write_32(XSPI_ADDR(XSPI_CMCFG1CS0_OFFSET), (0x0Au << XSPI_CMCFG1CS_RDLATE) | (0xA000u << XSPI_CMCFG1CS_RDCMD));
		/* Enable read access to CS0 for selected channel memory areas */
		mmio_write_32(XSPI_ADDR(XSPI_BMCTL0_OFFSET), (0x01u << XSPI_BMCTL0_CS0ACC));

		/* COMCFG:	use default values */
		/* BMCFG:	use default values */
		/* CSSCTL:	use default values */
	}
	break;

	default: {
		/* Error */
		panic();
	}
	break;
	}
}

static int32_t xspi_prepare(const uint32_t xspi_base)
{
	/* Check if transaction has already been requested. If so return error */
	if (0u != (mmio_read_32(XSPI_ADDR(XSPI_CDCTL0_OFFSET)) & XSPI_CDCTL0_TRREQ_MSK)) {
		return XSPI_ERROR;
	}

	mmio_write_32(XSPI_ADDR(XSPI_INTC_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_INTC_OFFSET)) | XSPI_INTC_CMDCMPC_MSK);
	mmio_write_32(XSPI_ADDR(XSPI_CDTBUF0_OFFSET), mmio_read_32(XSPI_ADDR(XSPI_CDTBUF0_OFFSET)) & (~(XSPI_CDTBUF_ADDSIZE_MSK | XSPI_CDTBUF_CMDSIZE_MSK)));

	return XSPI_SUCCESS;
}

static int xspi_read_status(const uint32_t xspi_base)
{
	volatile uint32_t status = 0xFFFFFFFF;

	st_xspi_cmd_info_t cmd_rdsta = {RDSTA, 0, 0};

	if (xspi_single_command(xspi_base, &cmd_rdsta) == XSPI_SUCCESS) {
		/* Command success */
		status = mmio_read_32(XSPI_ADDR(XSPI_CDD0BUF0_OFFSET));
	}

	return status;
}

uint32_t xspi_read_identification(const uint32_t xspi_base)
{
	uint32_t id = DEVICE_ID_BAD;
	uint32_t prev_id = DEVICE_ID_BAD;
	int32_t count = 1000;

	st_xspi_cmd_info_t cmd_rdid = {RDID, 0, 0};

	while (count > 0U) {
		if (xspi_single_command(xspi_base, &cmd_rdid) == XSPI_SUCCESS) {
			/* Command success */
			id = mmio_read_32(XSPI_ADDR(XSPI_CDD0BUF0_OFFSET)) & DEVID_ID_MASK;
			if ((id != DEVICE_ID_BAD) && (id != DEVICE_ID_ERROR) && (prev_id == id)) {
				/* Hardware ID is valid and has been repeated on two consecutive reads so exit the while loop and then function */
				break;
			}

			prev_id = id;
		}
		/* Allow retry of Id read in case Flash device has not had enough time to start-up*/
		count--;
	}

	return id;
}

int32_t xspi_setup(const uint32_t xspi_base, const xspi_modes_t xspi_mode)
{
	int32_t ret;

	/* xSPI Init */
	xspi_init_registers(xspi_base, xspi_mode);
	/* Initial flash prepare/check */
	ret = xspi_prepare(xspi_base);
	if (ret == XSPI_SUCCESS) {
		/* Reset the flash memory */
		ret = xspi_reset(xspi_base, xspi_mode);
		if (ret == XSPI_SUCCESS) {
			/* Read FLASH memory signature to check we can communicate */
			/* successfully (but don't check the ID) */
			uint32_t id = xspi_read_identification(xspi_base);

			if ((id == DEVICE_ID_BAD) || (id == DEVICE_ID_ERROR)) {
				/* Failed to read id */
				ret = XSPI_ERROR;
			}
		}
	}
	return ret;
}

int xspi_erase(const uint32_t xspi_base, const uintptr_t addr, uint32_t byte_count)
{
	int ret = XSPI_SUCCESS;
	uint32_t count = (byte_count >> 12) + 1;

	st_xspi_cmd_info_t cmd_wten = {WTEN, 0, 0};
	st_xspi_cmd_info_t cmd_erase = {ERASE, addr, 0};

	volatile uint32_t status = 0xFFFFFFFF;

	for (int i = 0; i < count; i++) {

		ret = xspi_single_command(xspi_base, &cmd_wten);
		if (ret != XSPI_SUCCESS)
			return ret;

		do {
			status = xspi_read_status(xspi_base);
		} while (0 == (status & 0x02));

		ret = xspi_single_command(xspi_base, &cmd_erase);
		if (ret != XSPI_SUCCESS)
			return ret;

		do {
			status = xspi_read_status(xspi_base);
		} while (0 != (status & 0x01));

		cmd_erase.addr = cmd_erase.addr + 0x1000;
	}

	return ret;
}

int xspi_write(const uint32_t xspi_base, const uintptr_t addr, uintptr_t data, uint32_t byte_count) {
    if (byte_count % sizeof(uint32_t) != 0) {
        return XSPI_ERROR;
    }

    uint32_t word_count = byte_count / sizeof(uint32_t);
    uint32_t* destination = (uint32_t*)(addr + XSPI_BASE_ADDRESS_OFFSET);
	st_xspi_cmd_info_t cmd_wten = {WTEN, 0, 0};
    const uint32_t* source = (const uint32_t*)data;
    int result = XSPI_SUCCESS;

    for (uint32_t i = 0; i < word_count; i++) {
        result = xspi_single_command(xspi_base, &cmd_wten);
        if (result != XSPI_SUCCESS) {
            return result;
        }

        // Enable memory acesss
        mmio_write_32(XSPI_ADDR(XSPI_BMCTL0_OFFSET), (0x03u << XSPI_BMCTL0_CS0ACC));
        
        *destination++ = *source++;
        
        while ((xspi_read_status(xspi_base) & XSPI_STATUS_BUSY_MASK) != 0) {
            // Wait for XSPI operation to complete
        }
    }

    return result;
}

int xspi_chip_erase(const uint32_t xspi_base)
{
	int ret;
	volatile uint32_t status = 0xFFFFFFFF;

	/* Issue the chip erase command */
	st_xspi_cmd_info_t cmd_wten = {RSTEN, 0, 0};
	st_xspi_cmd_info_t cmd_chip_erase = {CHIP_ERASE, 0, 0};

	ret = xspi_single_command(xspi_base, (&cmd_wten));
	if (ret == XSPI_SUCCESS) {
		ret = xspi_single_command(xspi_base, &cmd_chip_erase);
	}

	do {
		status = xspi_read_status(xspi_base);
	} while (0 != (status & 0x01));

	return ret;
}
