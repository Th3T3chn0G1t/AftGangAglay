/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_LOG_H
#define ASYS_LOG_H

#include <asys/base.h>
#include <asys/result.h>

/* NOTE: Handles errors internally to avoid nasty nested error states. */
void asys_log(const char*, const char*, ...);

/* NOTE: Does not log if `result == ASYS_RESULT_OK', */
void asys_log_result(const char*, const char*, enum asys_result);
void asys_log_result_path(
		const char*, const char*, const char*, enum asys_result);

#endif
