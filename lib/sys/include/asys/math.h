/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_MATH_H
#define ASYS_MATH_H

#include <asys/base.h>

#ifdef ASYS_STDC
# include <math.h>
# ifndef M_PI /* TODO: Remove reliance on this */
#  define M_PI (3.14159265358979323846)
# endif
#endif

#endif
