/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_IO_H
#define AGA_IO_H

#include <afeirsa/afeirsa.h>

/*
 * NOTE: Writes out a heap-alloc'd pointer to `ptr'.
 * (I know this is suboptimal IO management, but it's just handy to have
 * around. Let's hope the OS FS cache smiles down on us.)
 */
enum af_err aga_read(const char* path, af_uchar_t** ptr, af_size_t* size);

enum af_err aga_spawn_sync(const char* program, char** argv, const char* wd);

#ifdef AGA_HAVE_MAP
# define AGA_MK_LARGE_FILE_STRATEGY(path, ptr, size) \
		aga_fmap((path), (ptr), (size))
# define AGA_KILL_LARGE_FILE_STRATEGY(ptr, size) \
		aga_funmap((ptr), (size))
enum af_err aga_fmap(const char* path, af_uchar_t** ptr, af_size_t* size);
enum af_err aga_funmap(af_uchar_t* ptr, af_size_t size);
#else
# define AGA_MK_LARGE_FILE_STRATEGY(path, ptr, size) \
		aga_read((path), (ptr), (size))
# define AGA_KILL_LARGE_FILE_STRATEGY(ptr, size) \
		(free((ptr)), AF_ERR_NONE)
#endif

#endif
