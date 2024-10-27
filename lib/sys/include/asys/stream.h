/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+asys@pm.me>
 */

#ifndef ASYS_STREAM_H
#define ASYS_STREAM_H

#include <asys/base.h>
#include <asys/result.h>
#include <asys/filedata.h>

#ifdef ASYS_WIN32
# include <asys/sys/win32/streamdata.h>
#elif defined(ASYS_UNIX)
# include <asys/sys/unix/streamdata.h>
#elif defined(ASYS_STDC)
# include <asys/sys/stdc/streamdata.h>
/* TODO: #else RAM-disk support with an embedded pack? */
#endif

#define ASYS_COPY_ALL ((asys_size_t) -1)

enum asys_stream_whence {
	ASYS_SEEK_SET,
	ASYS_SEEK_END,
	ASYS_SEEK_CURRENT
};

enum asys_result asys_stream_new(struct asys_stream*, const char*);
enum asys_result asys_stream_delete(struct asys_stream*);

enum asys_result asys_stream_seek(
		struct asys_stream*, enum asys_stream_whence, asys_offset_t);

enum asys_result asys_stream_read(
		struct asys_stream*, asys_size_t*, void*, asys_size_t);

enum asys_result asys_stream_attribute(
		struct asys_stream*, enum asys_file_attribute_type,
		union asys_file_attribute*);

/* NOTE: No stream-writing IO functions are available outside of dev builds. */
enum asys_result asys_stream_write(
		struct asys_stream*, const void*, asys_size_t);

enum asys_result asys_stream_write_format(
		struct asys_stream*, const char*, ...);

enum asys_result asys_stream_write_characters(
		struct asys_stream*, char, asys_size_t);

/*
 * Respects initial stream positions.
 * `count == ASYS_COPY_ALL' results in a copy until EOF.
 */
enum asys_result asys_stream_splice(
		struct asys_stream*, struct asys_stream*, asys_size_t);

#endif
