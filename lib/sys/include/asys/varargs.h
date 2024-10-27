/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+asys@pm.me>
 */

#ifndef ASYS_VARARGS_H
#define ASYS_VARARGS_H

/* NOTE: It's unclear whether Windows 3.1 exposed `stdarg.h'. */
#ifdef __STDC__
# include <stdarg.h>
#else
# include <varargs.h>
#endif

#endif
