/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/log.h>
#include <asys/system.h>
#include <asys/string.h>
#include <asys/result.h>

/* TODO: Reintroduce log file later. */

/*
 * TODO: Rudimentary `$TERM' parsing and re-enable log colors.
#define NORMAL "\033[036m" "\033[0m"
#define WARNING "\033[033m" "\033[0m"
#define ERROR "\033[031m" "\033[0m"
 */

void asys_log(const char* file, const char* format, ...) {
	va_list list;

#ifdef ASYS_STDC
	if(printf("[%s] ", file) < 0) perror("printf");
#endif

	va_start(list, format);

#ifdef ASYS_STDC
	if(vprintf(format, list) < 0) perror("vprintf");
	if(putchar('\n') == EOF) perror("putchar");
#else
	/*
	 * TODO: Non-stdc implementations of log.
	 * 		 Windows should exclusively output to a logfile -- as "the Windows
	 * 		 Versions of the C run-time libraries exclude the [...] C run-time
	 * 		 console-input-and-output functions" (Win3 Guide, 14.5.5).
	 */
	if((result = asys_string_format_variadic(&buffer, format, list))) {
		return;
	}
#endif

	va_end(list);
}

void asys_log_result(
		const char* file, const char* function, enum asys_result result) {

	if(!result) return;

	asys_log(file, "err: %s: %s", function, asys_result_description(result));
}

void asys_log_result_path(
		const char* file, const char* function, const char* path,
		enum asys_result result) {

	if(!result) return;

	asys_log(
			file, "err: %s: %s `%s'",
			function, asys_result_description(result), path);
}
