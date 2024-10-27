/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+asys@pm.me>
 */

#include <asys/getopt.h>
#include <asys/base.h>

#ifndef ASYS_UNIX
/* Prefer system `getopt'. */
# include <port/getopt.c>
#endif
