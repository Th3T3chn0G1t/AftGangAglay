/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_WIN32_STREAMDATA_H
#define ASYS_WIN32_STREAMDATA_H

typedef HFILE asys_stream_native_t;

struct asys_stream {
	asys_stream_native_t fd;
};

#endif
