/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/memory.h>
#include <asys/system.h>
#include <asys/error.h>

void asys_memory_zero(void* pointer, asys_size_t count) {
/* TODO: Detect `bzero'. */
#ifdef ASYS_STDC
	memset(pointer, 0, count);
#else
	asys_size_t i;
	char* bytes = pointer;

	for(i = 0; i < count; ++i) bytes[i] = 0;
#endif
}

void asys_memory_copy(void* to, const void* from, asys_size_t count) {
#ifdef ASYS_STDC
	memcpy(to, from, count);
#else
	asys_size_t i;
	char* to_bytes = to;
	const char* from_bytes = from;

	for(i = 0; i < count; ++i) to_bytes[i] = from_bytes[i];
#endif
}

void* asys_memory_allocate(asys_size_t size) {
#ifdef ASYS_WIN32
	(void) size;

	return 0;
#elif defined(ASYS_STDC)
	void* pointer;

	if(!(pointer = malloc(size))) (void) asys_error_errno(__FILE__, "malloc");

	return pointer;
#elif defined(ASYS_UNIX)
	/* TODO: Was there a more *nix-y way to do allocations pre-std? */
	(void) size;

	return 0;
#else
	/* TODO: Heap implementation? */
	(void) size;

	return 0;
#endif
}

void* asys_memory_allocate_zero(asys_size_t count, asys_size_t size) {
#ifdef ASYS_WIN32
	(void) size;

	return 0;
#elif defined(ASYS_STDC)
	void* pointer;

	if(!(pointer = calloc(count, size))) {
		(void) asys_error_errno(__FILE__, "malloc");
	}

	return pointer;
#elif defined(ASYS_UNIX)
	(void) size;

	return 0;
#else
	(void) size;

	return 0;
#endif
}

void asys_memory_free(void* pointer) {
#ifdef ASYS_WIN32
	(void) pointer;
#elif defined(ASYS_STDC)
	free(pointer);
#elif defined(ASYS_UNIX)
	(void) pointer;
#else
	(void) pointer;
#endif
}

void* asys_memory_reallocate(void* pointer, asys_size_t size) {
#ifdef ASYS_WIN32
	(void) pointer;
	(void) size;

	return 0;
#elif defined(ASYS_STDC)
	if(!(pointer = realloc(pointer, size))) {
		(void) asys_error_errno(__FILE__, "realloc");
	}

	return pointer;
#elif defined(ASYS_UNIX)
	(void) pointer;
	(void) size;

	return 0;
#else
	(void) pointer;
	(void) size;

	return 0;
#endif
}

/* NOTE: Auto-frees the input pointer on OOM */
void* asys_memory_reallocate_safe(void* pointer, asys_size_t size) {
	void* new;

	if(!(new = asys_memory_reallocate(pointer, size))) {
		asys_memory_free(pointer);
	}

	return new;
}
