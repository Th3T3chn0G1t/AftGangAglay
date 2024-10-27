/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/result.h>
#include <asys/base.h>

const char* asys_result_description(enum asys_result e) {
	switch(e) {
		default:; ASYS_FALLTHROUGH;
			/* FALLTHROUGH */
		case ASYS_RESULT_OK: return "none";

		case ASYS_RESULT_ERROR: return "unknown";
		case ASYS_RESULT_EOF: return "end of file";
		case ASYS_RESULT_BAD_PARAM: return "bad parameter";
		case ASYS_RESULT_BAD_OP: return "bad operation";
		case ASYS_RESULT_BAD_SYNTAX: return "bad syntax";
		case ASYS_RESULT_INVALID_TOKEN: return "invalid token";
		case ASYS_RESULT_OOM: return "out of memory";
		case ASYS_RESULT_STACK_OVERFLOW: return "stack overflow";
		case ASYS_RESULT_STACK_UNDERFLOW: return "stack underflow";
		case ASYS_RESULT_NOT_IMPLEMENTED: return "not implemented";
		case ASYS_RESULT_MISSING_KEY: return "missing key";
		case ASYS_RESULT_BAD_TYPE: return "bad type";
	}
}
