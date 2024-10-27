/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <aga/io.h>
#include <asys/log.h>


#define AGA_WANT_WINDOWS_H
#include <aga/win32.h>

/* TODO: Implement BSD-y `<sys/dir.h>' `direct' interface  */
enum asys_result aga_directory_iterate(
		const char* path, aga_directory_callback_t fn, asys_bool_t recurse,
		void* pass, asys_bool_t keep_going) {

#ifdef _WIN32
	/*
	 * TODO: These compat. functions appear to have come about in 2000-ish with
	 * 		 XP -- what did 9x/3.1 use to access these "portably"?
	 * 		 There is some dubious evidence that the `_dos_' variants of these
	 * 		 Existed earlier -- including in `14.5.9' of Win3.1 Guide to
	 * 		 Programming:
	 * 		 		https://jeffpar.github.io/kbarchive/kb/043/Q43144/
	 * 		 		https://www.ee.iitb.ac.in/student/~bhush/C%20Programming%20-%20Just%20the%20FAQs.pdf
	 * 		 Possibly as a Watcom/Borland exclusive extension:
	 *				https://openwatcom.org/ftp/manuals/1.5/clib.pdf
	 *		 There's also something weird with the MSDN docs here:
	 *		 		https://learn.microsoft.com/en-us/cpp/c-runtime-library/system-calls?view=msvc-170
	 *		 As this set of functions are the only ones present.
	 */

	asys_fixed_buffer_t buffer = { 0 };

	enum asys_result result;
	enum asys_result held_result = ASYS_RESULT_OK;

	struct _finddata_t data;

#ifdef _WIN64
	intptr_t find;
#else
	long find;
#endif

	if(sprintf(buffer, "%s\\*", path) < 0) {
		return aga_error_system(__FILE__, "sprintf");
	}

	/* TODO: Should this class of DOS compat function use `_doserrno'? */
	if((find = _findfirst(buffer, &data)) == -1) {
		return aga_error_system_path(__FILE__, "_findfirst", path);
	}

	do {
		static asys_bool_t did_warn_deprecation = ASYS_FALSE;

		if(data.name[0] == '.') continue;

		if(!did_warn_deprecation) {
			/* TODO: Address this in next major release. */
			asys_log(
					__FILE__,
					"warn: Windows directory iteration currently replaces "
					"`\\' with `/' during path concatenation -- this will be "
					"changed in a future release when config can mark inputs "
					"as paths to auto-replace path separators");

			did_warn_deprecation = !did_warn_deprecation;
		}

		if(sprintf(buffer, "%s/%s", path, data.name) < 0) {
			result = aga_error_system(__FILE__, "sprintf");
			if(keep_going) {
				held_result = result;
				continue;
			}
			else return result;
		}

		if(data.attrib & _A_SUBDIR) {
			if(recurse) {
				result = aga_directory_iterate(
						buffer, fn, recurse, pass, keep_going);

				if(result) {
					if(keep_going) {
						asys_log_result_path(
								__FILE__, "aga_directory_iterate", buffer,
								result);

						held_result = result;
						continue;
					}
					else return result;
				}
			}
			else continue;
		}
		else if((result = fn(buffer, pass))) {
			if(keep_going) {
				asys_log_result_path(
						__FILE__, "aga_directory_iterate::<callback>", buffer,
						result);

				held_result = result;
				continue;
			}
			else return result;
		}
	} while(_findnext(find, &data) != -1);

	if(errno != ENOENT) return aga_error_system(__FILE__, "_findnext");

	return held_result;
#elif defined(AGA_HAVE_DIRENT)
	enum asys_result result;
	enum asys_result held_result = ASYS_RESULT_OK;

	DIR* d;
	struct dirent* ent;
	union aga_file_attribute attr;

	if(!(d = opendir(path))) {
		return aga_error_system_path(__FILE__, "opendir", path);
	}

	/* TODO: Leaky EH. */
	while((ent = readdir(d))) {
		asys_fixed_buffer_t buffer = { 0 };

		if(ent->d_name[0] == '.') continue;

		if(sprintf(buffer, "%s/%s", path, ent->d_name) < 0) {
			result = aga_error_system(__FILE__, "sprintf");
			if(keep_going) {
				held_result = result;
				continue;
			}
			else return result;
		}

		if((result = aga_file_attribute_path(buffer, AGA_FILE_TYPE, &attr))) {
			if(keep_going) {
				asys_log_result_path(
						__FILE__, "aga_file_attribute_path", buffer, result);

				held_result = result;
				continue;
			}
			else return result;
		}

		if(attr.type == AGA_FILE_DIRECTORY) {
			if(recurse) {
				result = aga_directory_iterate(
						buffer, fn, recurse, pass, keep_going);

				if(result) {
					if(keep_going) {
						asys_log_result_path(
								__FILE__, "aga_directory_iterate", buffer,
								result);

						held_result = result;
						continue;
					}
					else return result;
				}
			}
			else continue;
		}
		else if((result = fn(buffer, pass))) {
			if(keep_going) {
				asys_log_result_path(
						__FILE__, "aga_directory_iterate::<callback>", buffer,
						result);

				held_result = result;
				continue;
			}
			else return result;
		}
	}

	if(closedir(d) == -1) return aga_error_system(__FILE__, "closedir");

	return held_result;
#else
	(void) path;
	(void) fn;
	(void) recurse;
	(void) pass;
	(void) keep_going;

	return ASYS_RESULT_NOT_IMPLEMENTED;
#endif
}
