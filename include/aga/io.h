/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_IO_H
#define AGA_IO_H

#include <asys/base.h>
#include <asys/result.h>

typedef enum asys_result (*aga_directory_callback_t)(const char*, void*);

enum asys_result aga_directory_iterate(
		const char*, aga_directory_callback_t, asys_bool_t, void*, asys_bool_t);

#endif
