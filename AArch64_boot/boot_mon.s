/*
 * Copyright (c) 2015-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Loader
    LDR x0, =__STACKS_END__
    MSR SP_EL0,x0
    MSR SP_EL1,x0
    MSR SP_EL2,x0
    MOV sp,x0
    MSR ELR_EL1,x0
    MSR ELR_EL2,x0
    MSR ELR_EL3,x0
    MSR SPSR_EL1,x0
    MSR SPSR_EL2,x0
    MSR SPSR_EL3,x0

// Enable cache
	mrs     x0, sctlr_el3
	orr     x0, x0, #(0x1 << 12)
	orr     x0, x0, #(0x1 <<  1)
	orr     x0, x0, #(0x1 <<  3)
	msr     sctlr_el3, x0
	isb

// Clear bss section
	mov	W0, #0x0
	ldr	X1, =__BSS_START__
	ldr	X2, =__BSS_SIZE__
bss_loop:
	subs	X2, X2, #4
	bcc	bss_end
	str	W0, [X1, X2]
	b	bss_loop
bss_end:
	BL	Main
	.end
