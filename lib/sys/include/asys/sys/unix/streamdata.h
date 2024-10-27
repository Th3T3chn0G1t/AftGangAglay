/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_UNIX_STREAMDATA_H
#define ASYS_UNIX_STREAMDATA_H

struct asys_stream {
	int fd;
	/*
	 * TODO: In dev/debug mode, remember the original file's path and use the
	 * 		 `_path' variants of error functions in all stream IO functions.
	 */
};

#endif
