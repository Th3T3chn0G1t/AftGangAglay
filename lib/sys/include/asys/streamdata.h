/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+asys@pm.me>
 */

#ifndef ASYS_STREAMDATA_H
#define ASYS_STREAMDATA_H

#include <asys/base.h>

#ifdef ASYS_WIN32
# include <asys/sys/win32/streamdata.h>
#elif defined(ASYS_UNIX)
# include <asys/sys/unix/streamdata.h>
#elif defined(ASYS_STDC)
# include <asys/sys/stdc/streamdata.h>
#else
/* TODO: RAM-disk support with an embedded pack? */
typedef int asys_stream_native_t;

struct asys_stream {
	asys_stream_native_t handle;
};
#endif

#endif
