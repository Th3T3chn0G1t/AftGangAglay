/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/base.h>

#include <aga/window.h>
#include <aga/gl.h>

# include "sys/win32/window.h"

#ifdef ASYS_WIN32
# include "sys/win32/window.h"
#else
# include "sys/x/window.h"
#endif
