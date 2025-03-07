/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <agan/agan.h>
#include <agan/object.h>
#include <agan/io.h>
#include <agan/draw.h>
#include <agan/utility.h>
#include <agan/math.h>
#include <agan/editor.h>

#include <aga/draw.h>
#include <aga/config.h>
#include <aga/gl.h>
#include <aga/script.h>

#include <asys/log.h>
#include <asys/string.h>

/*
 * TODO: Switch to unchecked List/Tuple/String accesses for release/noverify
 * 		 Builds? Disable parameter type strictness for noverify+dist?
 * 		 Enforce in main Python as global switch?
 */

/* TODO: Mode disable error-to-exception propagation - "continue". */

/*
 * NOTE: We need a bit of global state here to get engine system contexts etc.
 * 		 Into script land because this version of Python's state is spread
 * 		 Across every continent.
 */
struct py_object* agan_dict = 0;

const char* agan_trans_components[3] = { "pos", "rot", "scale" };
const char* agan_conf_components[3] = { "Position", "Rotation", "Scale" };
const char* agan_xyz[3] = { "X", "Y", "Z" };
const char* agan_rgb[3] = { "R", "G", "B" };

enum asys_result aga_insertstr(const char* key, const char* value) {
	struct py_object* o;

	if(!(o = py_string_new(value))) return ASYS_RESULT_OOM;

	if(py_dict_insert(agan_dict, key, o) == -1) return ASYS_RESULT_ERROR;

	return ASYS_RESULT_OK;
}

enum asys_result aga_insertfloat(const char* key, double value) {
	struct py_object* o;

	if(!(o = py_float_new(value))) return ASYS_RESULT_OOM;

	if(py_dict_insert(agan_dict, key, o) == -1) return ASYS_RESULT_ERROR;

	return ASYS_RESULT_OK;
}

enum asys_result aga_insertint(const char* key, py_value_t value) {
	struct py_object* o;

	if(!(o = py_int_new(value))) return ASYS_RESULT_OOM;

	if(py_dict_insert(agan_dict, key, o) == -1) return ASYS_RESULT_ERROR;

	return ASYS_RESULT_OK;
}

enum asys_result aga_mkmod(struct py_env* env, void** dict) {
	enum asys_result result;

	/*
	 * TODO: Put each module in a "namespace" (i.e. `agan.io.getkey' etc.).
	 * 		 Similar to how `ed' is structured.
	 */
#define aga_(name) { #name, agan_##name }
	static const struct py_methodlist methods[] = {
			/* Input */
			aga_(getkey), aga_(getmotion), aga_(setcursor), aga_(getbuttons),
			aga_(getpos),

			/* Drawing */
			aga_(setcam), aga_(text), aga_(fogparam), aga_(fogcol),
			aga_(clear), aga_(mktrans), aga_(line3d), aga_(getflag),
			aga_(shadeflat), aga_(getpix), aga_(setflag),

			/* Miscellaneous */
			aga_(getconf), aga_(packlist), aga_(log), aga_(die), aga_(dt),
			aga_(strsplit),

			/* Objects */
			aga_(mkobj), aga_(inobj), aga_(putobj), aga_(killobj),
			aga_(objind), aga_(objtrans), aga_(objconf),

			/* Maths */
			aga_(bitand), aga_(bitshl), aga_(randnorm), aga_(bitor),

			{ 0, 0 } };
#undef aga_

	struct py_object* module = py_module_new_methods(env, "agan", methods);
	if(!module) return ASYS_RESULT_ERROR;

	agan_dict = ((struct py_module*) module)->attr;

	if((result = agan_draw_register(env))) return result;
	if((result = agan_io_register(env))) return result;
	if((result = agan_math_register(env))) return result;
	if((result = agan_misc_register(env))) return result;
	if((result = agan_ed_register(env))) return result;

	*dict = agan_dict;

	return ASYS_RESULT_OK;
}

asys_bool_t aga_script_err(const char* function, enum asys_result result) {
	static asys_fixed_buffer_t buffer = { 0 };

	const char* description;

	if(!result) return ASYS_FALSE;

	description = asys_result_description(result);

	result = asys_string_format(&buffer, 0, "%s: %s", function, description);
	asys_log_result(__FILE__, "asys_string_format", result);

	py_error_set_string(py_runtime_error, buffer);

	return ASYS_TRUE;
}

asys_bool_t aga_script_gl_err(const char* function) {
	return aga_script_err(function, aga_error_gl(__FILE__, function));
}

asys_bool_t agan_settransmat(struct py_object* trans, asys_bool_t inv) {
	struct py_object* comp;
	struct py_object* xo;
	struct py_object* yo;
	struct py_object* zo;
	double x, y, z;
	asys_size_t i;

	for(i = inv ? 2 : 0; i < 3; inv ? --i : ++i) {
		if(!(comp = py_dict_lookup(trans, agan_trans_components[i]))) {
			py_error_set_key();
			return ASYS_TRUE;
		}

		if((xo = py_list_get(comp, 0))->type != PY_TYPE_FLOAT) {
			py_error_set_badarg();
			return 0;
		}

		if((yo = py_list_get(comp, 1))->type != PY_TYPE_FLOAT) {
			py_error_set_badarg();
			return 0;
		}

		if((zo = py_list_get(comp, 2))->type != PY_TYPE_FLOAT) {
			py_error_set_badarg();
			return 0;
		}

		x = py_float_get(xo);
		y = py_float_get(yo);
		z = py_float_get(zo);

		switch(i) {
			default: break;
			case 0: {
				glTranslated(x, y, z);
				if(aga_script_gl_err("glTranslated")) return ASYS_TRUE;
				break;
			}
			case 1: {
				glRotated(x, 1.0, 0.0, 0.0);
				if(aga_script_gl_err("glRotated")) return ASYS_TRUE;
				glRotated(y, 0.0, 1.0, 0.0);
				if(aga_script_gl_err("glRotated")) return ASYS_TRUE;
				glRotated(z, 0.0, 0.0, 1.0);
				if(aga_script_gl_err("glRotated")) return ASYS_TRUE;
				break;
			}
			case 2: {
				glScaled(x, y, z);
				if(aga_script_gl_err("glScaled")) return ASYS_TRUE;
				break;
			}
		}
	}

	return ASYS_FALSE;
}

struct py_object* agan_scriptconf(
		struct aga_config_node* node, asys_bool_t root, struct py_object* list) {

	enum asys_result result;

	const char* str;
	struct aga_config_node* out;
	const char** names;
	unsigned i, len = py_varobject_size(list);
	struct py_object* retval;

	if(!(names = malloc(len * sizeof(char*)))) return py_error_set_nomem();

	for(i = 0; i < len; ++i) {
		struct py_object* op = py_list_get(list, i);

		if(op->type != PY_TYPE_STRING) {
			free(names);
			py_error_set_badarg();
			return 0;
		}

		names[i] = py_string_get(op);
	}

	result = aga_config_lookup_check(
			root ? node->children : node, names, len, &out);
	free(names);
	if(result) return py_object_incref(PY_NONE);

	str = out->data.string ? out->data.string : "";
	switch(out->type) {
		default:; ASYS_FALLTHROUGH;
		/* FALLTHROUGH */
		case AGA_NONE: retval = py_object_incref(PY_NONE); break;
		case AGA_STRING: retval = py_string_new(str); break;
		case AGA_INTEGER: retval = py_int_new(out->data.integer); break;
		case AGA_FLOAT: retval = py_float_new(out->data.flt); break;
	}

	if(!retval) py_error_set_nomem();

	return retval;
}
