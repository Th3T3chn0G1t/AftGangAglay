/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_STDC_STREAMDATA_H
#define ASYS_STDC_STREAMDATA_H

typedef void* asys_stream_native_t;

struct asys_stream {
	void* fp;
	/*
	 * TODO: In dev+debug builds -- remember the open mode for the stream and
	 * 		 Fail early with invalid operations.
	 */
};

#endif
