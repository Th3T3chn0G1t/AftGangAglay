/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/base.h>

#include <asys/stdc.h>

asys_native_long_t asys_native_strtol(const char* s, char** end, int base) {
#ifdef ASYS_STDC
# ifdef ASYS_LP32
	return strtoll(s, end, base);
# else
	return strtol(s, end, base);
# endif
#endif
}
