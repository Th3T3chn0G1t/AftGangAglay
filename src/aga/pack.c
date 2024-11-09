/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <aga/pack.h>

#include <asys/log.h>
#include <asys/memory.h>
#include <asys/string.h>

/*
 * TODO: Allow inplace use of UNIX `compress'/`uncompress' utilities on pack
 * 		 In distribution.
 * 		 Windows apparently had `COMPRESS.EXE' and could use `LZRead' etc. for
 * 		 In-place compressed reads (!).
 */

/*
 * TODO: Allow pack input as a "raw" argument and make space for a shebang so
 * 		 Packs can be executed directly by the shell if we're installed on the
 * 		 System.
 */

/* TODO: Hopefully we eventually won't need this anymore. */
struct aga_resource_pack* aga_global_pack = 0;

enum asys_result aga_resource_pack_lookup(
		struct aga_resource_pack* pack, const char* path,
		struct aga_resource** out) {

	asys_size_t i;

	if(!pack) return ASYS_RESULT_BAD_PARAM;
	if(!path) return ASYS_RESULT_BAD_PARAM;
	if(!out) return ASYS_RESULT_BAD_PARAM;

	for(i = 0; i < pack->count; ++i) {
		struct aga_resource* resource = &pack->resources[i];

		if(!resource->config) continue;

		if(asys_string_equal(resource->config->name, path)) {
			*out = &pack->resources[i];
			return ASYS_RESULT_OK;
		}
	}

	asys_log(__FILE__, "err: Path `%s' not found in resource pack", path);

	return ASYS_RESULT_BAD_PARAM;
}

enum asys_result aga_resource_pack_new(
		const char* path, struct aga_resource_pack* pack) {

	enum asys_result result;

	asys_size_t i;
	struct aga_resource_pack_header header;

	if(!path) return ASYS_RESULT_BAD_PARAM;
	if(!pack) return ASYS_RESULT_BAD_PARAM;

	aga_global_pack = pack;

	asys_memory_zero(pack, sizeof(struct aga_resource_pack));
	asys_memory_zero(&pack->root, sizeof(struct aga_config_node));

#ifndef NDEBUG
	pack->outstanding_refs = 0;
#endif

	asys_log(__FILE__, "Loading resource pack `%s'...", path);

	if((result = asys_stream_new(&pack->stream, path))) return result;

	result = asys_stream_read(
			&pack->stream, 0, &header,
			sizeof(struct aga_resource_pack_header));

	if(result) goto cleanup;

	if(header.magic != AGA_PACK_MAGIC) {
		asys_log(
				__FILE__,
				"Invalid resource pack magic. Expected `%x', got `%x'",
				AGA_PACK_MAGIC, header.magic);

		result = ASYS_RESULT_BAD_PARAM;
		goto cleanup;
	}

	result = aga_config_new(&pack->stream, header.size, &pack->root);
	if(result) goto cleanup;

	pack->count = pack->root.children->len;
	pack->data_offset = header.size + sizeof(header);

	pack->resources = asys_memory_allocate_zero(
			pack->count, sizeof(struct aga_resource));

	if(!pack->resources) {
		result = ASYS_RESULT_OOM;
		goto cleanup;
	}

	for(i = 0; i < pack->count; ++i) {
		static const char* offset_name = "Offset";
		static const char* size_name = "Size";

		struct aga_resource* resource = &pack->resources[i];
		struct aga_config_node* node = &pack->root.children->children[i];

		aga_config_int_t v;
		asys_size_t offset, size;

		result = aga_config_lookup(
				node, &offset_name, 1, &v, AGA_INTEGER, ASYS_TRUE);

		if(result) continue;
		offset = v;

		result = aga_config_lookup(
				node, &size_name, 1, &v, AGA_INTEGER, ASYS_TRUE);

		if(result) continue;
		size = v;

		/* Only make a valid resource entry once all checks have passed. */
		resource->config = node;
		resource->pack = pack;
		resource->offset = (asys_size_t) offset;
		resource->size = (asys_size_t) size;
	}

	asys_log(
			__FILE__,
			"Processed `" ASYS_NATIVE_ULONG_FORMAT "' resource entries",
			pack->count);

	return ASYS_RESULT_OK;

	cleanup: {
		asys_memory_free(pack->resources);

		asys_log_result(
				__FILE__, "asys_stream_delete",
				asys_stream_delete(&pack->stream));

		asys_log_result(
				__FILE__, "aga_config_delete",
				aga_config_delete(&pack->root));

		return result;
	}
}

enum asys_result aga_resource_pack_delete(struct aga_resource_pack* pack) {
	enum asys_result result;

	if(!pack) return ASYS_RESULT_BAD_PARAM;

	if((result = aga_resource_pack_sweep(pack))) return result;

#ifndef NDEBUG
	if(pack->outstanding_refs) {
		asys_log(
				__FILE__,
				"warn: `" ASYS_NATIVE_ULONG_FORMAT "' outstanding refs held in"
				" freed respack", pack->outstanding_refs);
	}
#endif

	asys_memory_free(pack->resources);

	result = asys_stream_delete(&pack->stream);
	if(result) {
		asys_log_result(
				__FILE__, "aga_config_delete", aga_config_delete(&pack->root));

		return result;
	}

	return aga_config_delete(&pack->root);
}

enum asys_result aga_resource_pack_sweep(struct aga_resource_pack* pack) {
	asys_size_t i;

	if(!pack) return ASYS_RESULT_BAD_PARAM;

	for(i = 0; i < pack->count; ++i) {
		struct aga_resource* resource = &pack->resources[i];

		if(resource->refcount || !resource->data) continue;

#ifndef NDEBUG
		pack->outstanding_refs--;
#endif

		asys_memory_free(resource->data);
		resource->data = 0;
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_resource_new(
		struct aga_resource_pack* pack, const char* path,
		struct aga_resource** resource) {

	enum asys_result result;

	if(!path) return ASYS_RESULT_BAD_PARAM;
	if(!pack) return ASYS_RESULT_BAD_PARAM;
	if(!resource) return ASYS_RESULT_BAD_PARAM;

	result = aga_resource_pack_lookup(pack, path, resource);
	if(result) {
		asys_log(__FILE__, "err: Failed to find resource `%s'", path);
		return result;
	}

	if(!(*resource)->data) {
		result = aga_resource_seek(*resource, 0);
		if(result) return result;

#ifndef NDEBUG
		pack->outstanding_refs++;
#endif

		/* TODO: Use mapping for large reads. */
		(*resource)->data = asys_memory_allocate((*resource)->size);
		if(!(*resource)->data) return ASYS_RESULT_OOM;

		result = asys_stream_read(
				&pack->stream, 0, (*resource)->data, (*resource)->size);

		if(result) return result;
	}

	++(*resource)->refcount;

	return ASYS_RESULT_OK;
}

enum asys_result aga_resource_stream(
		struct aga_resource_pack* pack, const char* path,
		struct asys_stream** stream, asys_size_t* size) {

	enum asys_result result;
	struct aga_resource* resource;

	if(!pack) return ASYS_RESULT_BAD_PARAM;
	if(!path) return ASYS_RESULT_BAD_PARAM;

	result = aga_resource_pack_lookup(pack, path, &resource);
	if(result) return result;

	result = aga_resource_seek(resource, 0);
	if(result) return result;

	*stream = &pack->stream;
	*size = resource->size;

	return ASYS_RESULT_OK;
}

enum asys_result aga_resource_seek(
		struct aga_resource* resource, struct asys_stream** stream) {

	int result;
	asys_offset_t offset;

	if(!resource) return ASYS_RESULT_BAD_PARAM;

	offset = (asys_offset_t) (resource->pack->data_offset + resource->offset);

	result = asys_stream_seek(&resource->pack->stream, ASYS_SEEK_SET, offset);
	if(result) return result;

	if(stream) *stream = &resource->pack->stream;

	return ASYS_RESULT_OK;
}

enum asys_result aga_resource_aquire(struct aga_resource* res) {
	if(!res) return ASYS_RESULT_BAD_PARAM;

	++res->refcount;

	return ASYS_RESULT_OK;
}

enum asys_result aga_resource_release(struct aga_resource* res) {
	if(!res) return ASYS_RESULT_BAD_PARAM;

	if(res->refcount) --res->refcount;

	return ASYS_RESULT_OK;
}
