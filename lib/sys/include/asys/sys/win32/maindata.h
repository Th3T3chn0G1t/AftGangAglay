/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_WIN32_MAINDATA_H
#define ASYS_WIN32_MAINDATA_H

struct asys_main_data {
	void* module;
	void* window_class;
	int show;

	int argc;
	char** argv;
};

#endif
