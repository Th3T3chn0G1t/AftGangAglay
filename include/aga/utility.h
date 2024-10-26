/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_UTILITY_H
#define AGA_UTILITY_H

#include <aga/environment.h>
#include <aga/result.h>

#define AGA_SWAP_FLOAT(a, b) \
	do { \
		float scratch = b; \
		b = a; \
		a = scratch; \
	} while(0)

void* aga_memset(void*, int, aga_size_t);
void* aga_bzero(void*, aga_size_t);
void* aga_memcpy(void*, const void*, aga_size_t);

aga_bool_t aga_streql(const char*, const char*);
aga_bool_t aga_strneql(const char*, const char*, aga_size_t);
aga_size_t aga_strlen(const char*);

char* aga_getenv(const char*);

/*
 * NOTE: `aga_realloc' frees the in pointer on error. This is different from
 * 		 Normal `realloc' but keeping the original pointer on error isn't
 * 		 Something we really need - it complicates EH.
 */
/*
 * NOTE: EH for these is still propagated via. errno for simplicity. Any
 * 		 Alternative impls should set errno in these wrappers if they don't
 * 		 Do so natively.
 */
/*
 * TODO: Massively inconsistent between using errno and just returning OOM on
 * 		 Failure for these.
 */
void* aga_malloc(aga_size_t);
void* aga_calloc(aga_size_t, aga_size_t);
void* aga_realloc(void*, aga_size_t);
void aga_free(void*);

char* aga_strdup(const char*);

#endif
