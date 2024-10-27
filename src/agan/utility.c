/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <agan/utility.h>

#include <aga/startup.h>
#include <asys/log.h>
#include <aga/script.h>

#include <apro.h>

enum asys_result agan_misc_register(struct py_env* env) {
	enum asys_result result;

	(void) env;

	if((result = aga_insertstr("VERSION", AGA_VERSION))) return result;

#ifdef _WIN32
	if((result = aga_insertstr("PLATFORM", "win32"))) return result;
#else
	if((result = aga_insertstr("PLATFORM", "x"))) return result;
#endif

	/* TODO: Var to detect dev builds. */
#ifndef NDEBUG
	if((result = aga_insertstr("MODE", "debug"))) return result;
#else
	if((result = aga_insertstr("MODE", "release"))) return result;
#endif

	/* TODO: BSD-y builds some day -- `bmake' support etc. */
	/* TODO: IRIX-y builds some day -- IRIX `make' support etc. */
#ifdef _MSC_VER
	if((result = aga_insertstr("CENV", "vc"))) return result;
#elif defined(__GNUC__)
	if((result = aga_insertstr("CENV", "gnu"))) return result;
#else
	if((result = aga_insertstr("CENV", "std"))) return result;
#endif

	return ASYS_RESULT_OK;
}

struct py_object* agan_getconf(
		struct py_env* env, struct py_object* self, struct py_object* args) {

	struct aga_settings* opts = AGA_GET_USERDATA(env)->opts;
	struct py_object* v;

	(void) env;
	(void) self;

	apro_stamp_start(APRO_SCRIPTGLUE_GETCONF);

	/* getconf(list[...]) */
	if(!aga_arg_list(args, PY_TYPE_LIST)) {
		return aga_arg_error("getconf", "list");
	}

	v = agan_scriptconf(&opts->config, ASYS_TRUE, args);

	apro_stamp_end(APRO_SCRIPTGLUE_GETCONF);

	return v;
}

static void agan_log_object(struct py_object* op, const char* file) {
	switch(op->type) {
		default: {
			/* TODO: Re-implement `str()` for all objects. */
			asys_log(file, "<object @%p>", (void*) op);
			break;
		}

		case PY_TYPE_STRING: {
			asys_log(file, py_string_get(op));
			break;
		}

		/* TODO: Implement `py_string_cat' of non-string objects. */
		case PY_TYPE_FLOAT: {
			asys_log(file, "%lf", py_float_get(op));
			break;
		}

		case PY_TYPE_INT: {
			asys_log(file, "%llu", py_int_get(op));
			break;
		}
	}
}

struct py_object* agan_log(
		struct py_env* env, struct py_object* self, struct py_object* args) {

	unsigned i;
	const char* file;

	(void) env;
	(void) self;

	apro_stamp_start(APRO_SCRIPTGLUE_LOG);

	/* log(object...) */
	if(!args) {
		return aga_arg_error("log", "object...");
	}

	file = py_string_get(env->current->code->filename);

	if(py_is_varobject(args) && args->type != PY_TYPE_STRING) {
		for(i = 0; i < py_varobject_size(args); ++i) {
			/*
			 * TODO: Single line logging instead of the series of logs that
			 *       This produces.
			 */
			if(args->type == PY_TYPE_LIST) {
				agan_log_object(py_list_get(args, i), file);
			}
			else if(args->type == PY_TYPE_TUPLE) {
				agan_log_object(py_tuple_get(args, i), file);
			}
		}
	}
	else agan_log_object(args, file);

	apro_stamp_end(APRO_SCRIPTGLUE_LOG);

	return py_object_incref(PY_NONE);
}

struct py_object* agan_die(
		struct py_env* env, struct py_object* self, struct py_object* args) {

	asys_bool_t* die = AGA_GET_USERDATA(env)->die;

	(void) env;
	(void) self;

	apro_stamp_start(APRO_SCRIPTGLUE_DIE);

	if(args) return aga_arg_error("die", "none");

	*die = ASYS_TRUE;

	apro_stamp_end(APRO_SCRIPTGLUE_DIE);

	return py_object_incref(PY_NONE);
}

struct py_object* agan_dt(
		struct py_env* env, struct py_object* self, struct py_object* args) {

	(void) env;
	(void) self;

	if(args) return aga_arg_error("dt", "none");

	return py_int_new(*AGA_GET_USERDATA(env)->dt);
}
