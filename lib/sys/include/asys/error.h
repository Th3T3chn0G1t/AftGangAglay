/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_ERROR_H
#define ASYS_ERROR_H

#include <asys/base.h>
#include <asys/result.h>

ASYS_NORETURN void asys_abort(void);

ASYS_NORETURN void asys_result_fatal(
		const char*, const char*, enum asys_result);

void asys_result_check(const char*, const char*, enum asys_result);
void asys_result_check_path(
		const char*, const char*, const char*, enum asys_result);

/*
 * NOTE: `GetLastError' did not exist in 1992 so there are no pure-Windows
 * 		  Equivalents of `errno' code. Individual subsystems can have their own
 * 		  Error codes.
 */

/* NOTE: Pass null to `file' to suppress error message printout. */
#ifdef ASYS_STDC
enum asys_result asys_result_errno(const char*, const char*);
enum asys_result asys_result_errno_path(const char*, const char*, const char*);
#endif

#endif
