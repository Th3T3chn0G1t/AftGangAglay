/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_PACK_H
#define AGA_PACK_H

#include <aga/config.h>
#include <asys/result.h>

#include <asys/stream.h>

#define AGA_PACK_MAGIC (0xA6AU)

struct aga_resource_pack;

typedef float aga_model_tail_t[6];
typedef asys_uint_t aga_image_tail_t;

struct aga_resource_pack_header {
	asys_uint_t size;
	asys_uint_t magic;
};

struct aga_resource {
	asys_size_t refcount;
	/*
	 * TODO: This should be an fpos, not an offset. May need to restructure
	 * 		 Pack config entries to point to sequential offsets instead of
	 * 		 Absolute offsets.
	 */
	asys_offset_t offset; /* Offset into pack data fields, not data member. */

	void* data;
	asys_size_t size;

	struct aga_resource_pack* pack;

	struct aga_config_node* config;
};

struct aga_resource_pack {
	struct asys_stream stream;
	asys_size_t data_offset;

	/*
	 * TODO: This should eventually be a hashmap. Windows has `GetAtom' etc. as
	 * 		 As a sort of built-in hashmap system -- does X allow arbitrary use
	 * 		 Of Atoms (and is it wise to do so?). If so, then add natively to
	 * 		 Python to avoid re-creating loads of strings.
	 */
	struct aga_resource* resources;
	asys_size_t count; /* Alias for `pack->root.children->len'. */

	/* TODO: This should be enabled for dev builds, not just debug builds. */
#ifndef NDEBUG
	asys_size_t outstanding_refs;
#endif

	struct aga_config_node root;
};

/*
 * TODO: This is only for situations where we can't get the context through
 *		 Non-global data flow (i.e. filesystem intercepts). Once we have a
 *		 More congruent state model for Python etc. we can
 */
extern struct aga_resource_pack* aga_global_pack;

enum asys_result aga_resource_pack_new(const char*, struct aga_resource_pack*);
enum asys_result aga_resource_pack_delete(struct aga_resource_pack*);

enum asys_result aga_resource_pack_lookup(
		struct aga_resource_pack*, const char*, struct aga_resource**);

enum asys_result aga_resource_pack_sweep(struct aga_resource_pack*);

/* Also counts as an acquire - i.e. initial refcount is 1. */
enum asys_result aga_resource_new(
		struct aga_resource_pack*, const char*, struct aga_resource**);

enum asys_result aga_resource_stream(
		struct aga_resource_pack*, const char*, struct asys_stream**,
		asys_size_t*);

enum asys_result aga_resource_seek(struct aga_resource*, struct asys_stream**);

/*
 * NOTE: You should ensure that you acquire after any potential error
 * 		 Conditions during object init, and before any potential error
 * 		 Conditions during object destroy in order to avoid holding onto refs
 * 		 For invalid objects.
 */
enum asys_result aga_resource_aquire(struct aga_resource*);
enum asys_result aga_resource_release(struct aga_resource*);

#endif
