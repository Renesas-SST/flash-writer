/*
 * Copyright (c) 2015-2017, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**********************
* COMMAND_UNITE       *
***********************/
#define		COMMAND_UNIT		30
#define		NORMAL_END		0
#define		ERROR_END		1

/**********************
* MESSAGE LINE       *
***********************/
#define		START_MESS_MON_LINE	2
#define		ALL_HELP_MESS_LINE	30

/**********************
* TABLE PARAMETER     *
***********************/
#define		TBL_END				0
#define		MESSAGE_END			0
#define		DISABLE				0x00
#define		ENABLE				0x01

/****************************************************************/
/*                                                              */
/*   Command Table Struct                                       */
/*                                                              */
/****************************************************************/
typedef struct protect_add {
	long		startAdd;
	long		endAdd;
} protect_add;

typedef struct com_menu {
	char		*comStr;
	void		(*comProg)();	 /* Program Pointer	*/
	protect_add *protectAdd;
} com_menu;
