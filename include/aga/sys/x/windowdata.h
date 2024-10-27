/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */
#ifndef AGA_X_WINDOWDATA_H
#define AGA_X_WINDOWDATA_H

typedef unsigned long aga_xid_t;

struct aga_window {
	asys_size_t width, height;

	aga_xid_t window;
	void* glx;

	asys_bool_t double_buffered;

	aga_xid_t blank_cursor, arrow_cursor;
};

struct aga_window_device {
	int screen;
	void* display;

	aga_xid_t wm_protocols, wm_delete;

	struct aga_window* capture;
};

#endif
