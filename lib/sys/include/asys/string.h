/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_STRING_H
#define ASYS_STRING_H

#include <asys/base.h>
#include <asys/result.h>

asys_bool_t asys_character_blank(char);

asys_size_t asys_string_length(const char*);

asys_bool_t asys_string_equal(const char*, const char*);
void asys_string_concatenate(char*, const char*);

/* NOTE: In case of null return, assume `ASYS_RESULT_OOM' error value. */
char* asys_string_duplicate(const char*);

asys_native_long_t asys_string_to_native_long(const char*);
double asys_string_to_double(const char*);

enum asys_result asys_string_format(
		asys_fixed_buffer_t*, asys_size_t*, const char*, ...);

#ifdef ASYS_VARARGS_H
enum asys_result asys_string_format_variadic(
		asys_fixed_buffer_t*, asys_size_t*, const char*, va_list);
#endif

#endif
