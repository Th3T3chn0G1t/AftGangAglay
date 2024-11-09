/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_MAIN_H
#define ASYS_MAIN_H

#include <asys/base.h>
#include <asys/result.h>

#ifdef ASYS_WIN32
# include <asys/sys/win32/maindata.h>
#else
# include <asys/sys/stdc/maindata.h>
#endif

#ifdef ASYS_WIN32
enum asys_result asys_win32_register_class(void*, void*);
#endif

enum asys_result asys_main(struct asys_main_data*);

#endif
