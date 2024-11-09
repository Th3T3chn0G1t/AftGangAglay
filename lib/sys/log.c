/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/log.h>
#include <asys/system.h>
#include <asys/string.h>
#include <asys/result.h>
#include <asys/error.h>

/* TODO: Reintroduce log file later. */

/*
 * TODO: Rudimentary `$TERM' parsing and re-enable log colors.
#define NORMAL "\033[036m" "\033[0m"
#define WARNING "\033[033m" "\033[0m"
#define ERROR "\033[031m" "\033[0m"
 */

void asys_log(const char* file, const char* format, ...) {
#ifdef ASYS_WIN32
	/* NOTE: This is not ideal but `OutputDebugString' is not great. */
	static asys_fixed_buffer_t body, buffer;
	asys_size_t length;
#endif

	va_list list;

	va_start(list, format);

#ifdef ASYS_STDC
	if(printf("[%s] ", file) < 0) perror("printf");
	if(vprintf(format, list) < 0) perror("vprintf");
	if(putchar('\n') == EOF) perror("putchar");
#elif defined(ASYS_WIN32)
	asys_string_format_variadic(&body, 0, format, list);
	asys_string_format(&buffer, &length, "[%s] %s", file, body);

	OutputDebugString(buffer);

	/*
	 * NOTE: Having console output isn't era-accurate -- but debugging
	 * 		 Heisenbugs without debug output is almost impossible.
	 */
# if defined(ASYS_WIN64) && (defined(AGA_DEVBUILD) || !defined(NDEBUG))
	{
		static asys_bool_t initialized = ASYS_FALSE;
		static HANDLE stdout_handle = 0;

		if(!initialized) {
			initialized = ASYS_TRUE;
			if(AllocConsole()) {
				stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
			}
		}

		do {
			if(!stdout_handle) break;

			WriteConsole(stdout_handle, buffer, length, 0, 0);
			WriteConsole(stdout_handle, "\r\n", sizeof("\r\n") - 1, 0, 0);
		} while(0);
	}
# endif
#else
	(void) file;
	(void) format;
#endif

	va_end(list);
}

void asys_log_result(
		const char* file, const char* function, enum asys_result result) {

	static asys_fixed_buffer_t buffer = { 0 };

	if(!result) return;

	asys_result_format(&buffer, function, 0, result);

	asys_log(file, buffer);
}

void asys_log_result_path(
		const char* file, const char* function, const char* path,
		enum asys_result result) {

	static asys_fixed_buffer_t buffer = { 0 };

	if(!result) return;

	asys_result_format(&buffer, function, path, result);

	asys_log(file, buffer);
}
