/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <aga/script.h>
#include <aga/pack.h>

#include <agan/agan.h>

#include <asys/log.h>
#include <asys/string.h>

/*
 * Defines a few misc. functions Python wants and a few scriptglue helpers
 * Declared in `agapyinc'.
 */

void py_fatal(const char* msg) {
	asys_log(__FILE__, "Python Fatal Error: %s", msg);
	abort();
}

enum asys_result py_open_r(const char* path, struct asys_stream** stream) {
	enum asys_result result;

	asys_size_t i;

	for(i = 0; i < aga_global_pack->count; ++i) {
		struct aga_resource* resource = &aga_global_pack->resources[i];

		if(!resource->config) continue;

		if(asys_string_equal(path, resource->config->name)) {
			result = aga_resource_seek(resource, stream);
			if(result) return result;

			return ASYS_RESULT_OK;
		}
	}

	return ASYS_RESULT_MISSING_KEY;
}
/*void* py_open_r(const char* path) {
	struct asys_stream* stream;
	asys_size_t i;

	for(i = 0; i < aga_global_pack->count; ++i) {
		struct aga_resource* resource = &aga_global_pack->resources[i];

		if(!resource->config) continue;

		if(asys_string_equal(path, resource->config->name)) {
			enum asys_result result;

			result = aga_resource_seek(resource, &stream);
			if(result) {
				asys_log_result(__FILE__, "aga_resource_seek", result);
				return 0;
			}

			return stream->fp;
		}
	}

	return 0;
}*/

asys_bool_t aga_arg_list(
		const struct py_object* args, enum py_type type) {

	return args && args->type == type;
}

asys_bool_t aga_vararg_list(
		const struct py_object* args, enum py_type type, asys_size_t len) {

	return aga_arg_list(args, type) && py_varobject_size(args) == len;
}

asys_bool_t aga_vararg_list_typed(
		const struct py_object* args, enum py_type type, asys_size_t len,
		enum py_type membtype) {

	unsigned i;
	asys_bool_t b = aga_vararg_list(args, type, len);

	if(!b) return ASYS_FALSE;

	for(i = 0; i < len; ++i) {
		if(type == PY_TYPE_LIST) {
			if(py_list_get(args, i)->type != membtype) return ASYS_FALSE;
		}
		else if(type == PY_TYPE_TUPLE) {
			if(py_tuple_get(args, i)->type != membtype) return ASYS_FALSE;
		}
	}

	return ASYS_TRUE;
}

asys_bool_t aga_arg(
		struct py_object** v, struct py_object* args, asys_size_t n,
		enum py_type type) {

	return (*v = py_tuple_get(args, (unsigned) n)) && (*v)->type == type;
}

asys_bool_t aga_vararg(
		struct py_object** v, struct py_object* args, asys_size_t n,
		enum py_type type, asys_size_t len) {

	return aga_arg(v, args, n, type) && py_varobject_size(*v) == len;
}

asys_bool_t aga_vararg_typed(
		struct py_object** v, struct py_object* args, asys_size_t n,
		enum py_type type, asys_size_t len, enum py_type membtype) {

	unsigned i;
	asys_bool_t b = aga_vararg(v, args, n, type, len);

	if(!b) return ASYS_FALSE;

	for(i = 0; i < len; ++i) {
		if(type == PY_TYPE_LIST) {
			if(py_list_get(*v, i)->type != membtype) return ASYS_FALSE;
		}
		else if(type == PY_TYPE_TUPLE) {
			if(py_tuple_get(*v, i)->type != membtype) return ASYS_FALSE;
		}
	}

	return ASYS_TRUE;
}

void* aga_arg_error(const char* function, const char* types) {
	asys_fixed_buffer_t buffer = { 0 };

	strcat(buffer, function);
	strcat(buffer, "() arguments must be ");
	strcat(buffer, types);
	py_error_set_string(py_type_error, buffer);

	return 0;
}
