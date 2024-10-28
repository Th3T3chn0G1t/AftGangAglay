/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/system.h>
#include <asys/string.h>
#include <asys/memory.h>
#include <asys/error.h>

asys_bool_t asys_character_blank(char character) {
#ifdef ASYS_WIN32
	(void) character;

	return ASYS_FALSE;
#elif defined(ASYS_STDC)
	return isspace(character);
#else
	return c == ' ' || c == '\r' || c == '\t' || c == '\n';
#endif
}

asys_size_t asys_string_length(const char* string) {
#ifdef ASYS_WIN32
	return (asys_size_t) lstrlen(string);
#elif defined(ASYS_STDC)
	return strlen(string);
#else
	asys_size_t length;

	for(length = 0; string[length]; ++length) continue;

	return length;
#endif
}

asys_bool_t asys_string_equal(const char* a, const char* b) {
#ifdef ASYS_STDC
	return !strcmp(a, b);
#else
	asys_size_t i = 0;

	while(ASYS_TRUE) {
		if(a[i] != b[i]) return ASYS_FALSE;
		if(!a[i]) return ASYS_TRUE;

		++i;
	}
#endif
}

void asys_string_concatenate(char* to, const char* from) {
#ifdef ASYS_STDC
	strcat(to, from);
#else
	(void) to;
	(void) from;
#endif
}

char* asys_string_duplicate(const char* string) {
	/* TODO: Detect impl. native `strdup'. */

	asys_size_t length = asys_string_length(string);
	char* new = asys_memory_allocate(length + 1);

	asys_memory_copy(new, string, length + 1);

	return new;
}

asys_native_long_t asys_string_to_native_long(const char* string) {
/* TODO: Did Windows 3.1 have a native way of doing this? */
/* TODO: Should we normalise `strtol' behaviours with our simpler impl.? */
#ifdef ASYS_STDC
# ifdef ASYS_LP32
	return strtoll(string, 0, 0);
# else
	return strtol(string, 0, 0);
# endif
#else
	asys_size_t i;
	asys_size_t len = asys_string_length(string);
	asys_native_long_t ret = 0;
	asys_bool_t negate = (string[0] == '-');

	for(i = len; i > (negate ? 1 : 0); --i) {
		asys_native_long_t v;
		char c = string[i - 1];

		if(c < '0' || c > '9') break;

		v = (asys_native_long_t) ((len - i) * 10);
		ret += (c - '0') * v;
	}

	return negate ? -ret : ret;
#endif
}

double asys_string_to_double(const char* string) {
#ifdef ASYS_STDC
	return strtod(string, 0);
#else
	(void) string;

	return 0.0;
#endif
}

enum asys_result asys_string_format(
		asys_fixed_buffer_t* buffer, asys_size_t* format_count,
		const char* format, ...) {

	enum asys_result result;
	va_list list;

	va_start(list, format);

	result = asys_string_format_variadic(buffer, format_count, format, list);

	va_end(list);

	return result;
}

enum asys_result asys_string_format_variadic(
		asys_fixed_buffer_t* buffer, asys_size_t* format_count,
		const char* format, va_list list) {

#ifdef ASYS_WIN32
	/* TODO: MSDN makes it unclear if this is the correct error condition. */
	int result = wvsprintf(*buffer, format, list);

	if(format_count) *format_count = result;

	if(result < 0) return ASYS_RESULT_ERROR;

	return ASYS_RESULT_OK;
#elif defined(ASYS_STDC)
	int result = vsprintf(*buffer, format, list);

	if(format_count) *format_count = result;

	if(result < 0) return asys_result_errno(__FILE__, "vsprintf");

	return ASYS_RESULT_OK;
#else
	/* TODO: Roll our own format. */
	(void) buffer;
	(void) format_count;
	(void) format;
	(void) list;

	return ASYS_RESULT_NOT_IMPLEMENTED;
#endif
}
