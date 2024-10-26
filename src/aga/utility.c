/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <aga/utility.h>
#include <aga/std.h>

void* aga_memset(void* p, int c, aga_size_t n) {
	return memset(p, c, n);
}

void* aga_bzero(void* p, aga_size_t n) {
	/* TODO: Era-accurate `bzero' detection. */
	return aga_memset(p, 0, n);
}

void* aga_memcpy(void* to, const void* from, aga_size_t sz) {
	return memcpy(to, from, sz);
}

aga_bool_t aga_streql(const char* a, const char* b) {
	return !strcmp(a, b);
}

aga_bool_t aga_strneql(const char* a, const char* b, aga_size_t sz) {
	return !strncmp(a, b, sz);
}

aga_size_t aga_strlen(const char* s) {
	return strlen(s);
}

/* TODO: Overridable allocator. */

void* aga_malloc(aga_size_t sz) {
	return malloc(sz);
}

char* aga_getenv(const char* s) {
	return getenv(s);
}

void* aga_calloc(aga_size_t n, aga_size_t sz) {
	return calloc(n, sz);
}

void* aga_realloc(void* p, aga_size_t sz) {
	void* new = realloc(p, sz);
	if(!new) free(p);
	return new;
}

void aga_free(void* p) {
	free(p);
}

char* aga_strdup(const char* s) {
	aga_size_t l = aga_strlen(s);
	char* ret = aga_malloc(l + 1);
	return aga_memcpy(ret, s, l + 1);
}
