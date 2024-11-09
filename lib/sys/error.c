/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/error.h>
#include <asys/log.h>
#include <asys/system.h>
#include <asys/string.h>

/*
 * TODO: Implement user-friendly error dialogs.
static const char msg[] =
	"AftGangAglay has encountered a fatal error and cannot continue.\n"
	"Would you like to report the issue?\n"
	"Please include the file `aga.log' along with your report.";
static const char report_uri[] =
	"https://github.com/AftGangAglay/AftGangAglay/issues/new";

asys_bool_t res;
(void) aga_dialog(msg, "Fatal Error", &res, ASYS_TRUE);

if(res) (void) aga_shell_open(report_uri);

exit(EXIT_FAILURE);
 */

void asys_result_format(
		asys_fixed_buffer_t* buffer, const char* function, const char* path,
		enum asys_result result) {

	if(path) {
		asys_string_format(
				buffer, 0, "err: %s: %s `%s'",
				function, asys_result_description(result), path);
	}
	else {
		asys_string_format(
				buffer, 0, "err: %s: %s",
				function, asys_result_description(result));
	}
}

void asys_result_fatal(
		const char* file, const char* function, enum asys_result result) {

	asys_result_fatal_path(file, function, 0, result);
}

void asys_result_fatal_path(
		const char* file, const char* function, const char* path,
		enum asys_result result) {

#ifdef ASYS_WIN32
	static asys_fixed_buffer_t buffer = { 0 };

	asys_result_format(&buffer, function, path, result);

	asys_log(file, buffer);
	FatalAppExit(0, buffer);
#else
	asys_log_result_path(file, function, path, result);
# ifdef ASYS_STDC
#  ifdef NDEBUG
	exit(EXIT_FAILURE);
#  else
	abort();
#  endif
# else
	/* TODO: What now? */
# endif
#endif
}

void asys_result_check(
		const char* file, const char* function, enum asys_result result) {

	if(!result) return;

	asys_result_fatal(file, function, result);
}

void asys_result_check_path(
		const char* file, const char* function, const char* path,
		enum asys_result result) {

	if(!result) return;

	asys_result_fatal_path(file, function, path, result);
}

#ifdef ASYS_STDC
enum asys_result asys_result_errno(const char* file, const char* function) {
	return asys_result_errno_path(file, function, 0);
}

enum asys_result asys_result_errno_path(
		const char* file, const char* function, const char* path) {

	if(file) {
		if(path) {
			asys_log(
					file, "err: %s: %s `%s'",
					function, strerror(errno), path);
		}
		else {
			asys_log(
					file, "err: %s: %s",
					function, strerror(errno));
		}
	}

	switch(errno) {
		default: return ASYS_RESULT_ERROR;
		case 0: return ASYS_RESULT_OK;

		/* TODO: More for "failed to open file" etc. */
# ifdef EBADF
			case EBADF: return ASYS_RESULT_BAD_PARAM;
# endif
# ifdef ENOMEM
			case ENOMEM: return ASYS_RESULT_OOM;
# endif
# ifdef EACCES
			case EACCES: return ASYS_RESULT_BAD_OP;
# endif
# ifdef EOPNOTSUPP
			case EOPNOTSUPP: return ASYS_RESULT_NOT_IMPLEMENTED;
# endif
	}
}
#endif
