/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/file.h>
#include <asys/system.h>
#include <asys/error.h>
#include <asys/stream.h>
#include <asys/log.h>

#ifdef ASYS_UNIX
# include "sys/unix/detail.h"
#endif

/*
 * TODO: Old stdc-y stat emulation -- reuse me.
static enum asys_result asys_file_attribute_type(
		const char* path, union aga_file_attr* out) {

# ifdef EISDIR
 * Try and fall back to a solution using `fopen'.
FILE* f;

if(!(f = fopen(path, "r"))) {
if(errno == EISDIR) *isdir = ASYS_TRUE;
else return aga_error_system_path(__FILE__, "fopen", path);
}
else *isdir = ASYS_FALSE;

if(fclose(f) == EOF) {
return aga_error_system(__FILE__, "fclose");
}

return ASYS_RESULT_OK;
# else
return AGA_ERROR_NOT_IMPLEMENTED;
# endif
}

enum asys_result asys_file_attribute_length(void* fp, asys_size_t* size) {
	long off;
	long tell;

	if((off = ftell(fp)) == -1) return aga_error_system(__FILE__, "ftell");

	if(fseek(fp, 0, SEEK_END)) {
		return aga_error_system(__FILE__, "fseek");
	}
	if((tell = ftell(fp)) == -1) return aga_error_system(__FILE__, "ftell");
	*size = (asys_size_t) tell;

	if(fseek(fp, off, SEEK_SET)) {
		return aga_error_system(__FILE__, "fseek");
	}

	return ASYS_RESULT_OK;
}

static enum asys_result asys_file_attribute_select(
		void* fp, enum asys_file_attribute_type attr, union asys_file_attribute* out) {

	switch(attr) {
		default: return ASYS_RESULT_BAD_PARAM;

		case AGA_FILE_MODIFIED: return ASYS_RESULT_NOT_IMPLEMENTED;

		case ASYS_FILE_LENGTH: return asys_file_attribute_length(fp, &out->length);

 * If it's a file handle, it's a regular file.
		case ASYS_FILE_TYPE: {
			out->type = AGA_REGULAR;
			break;
		}
	}

	return ASYS_RESULT_OK;
}
#endif

enum asys_result asys_path_attribute(
		const char* path, enum asys_file_attribute_type attr,
		union asys_file_attribute* out) {

	if(!path) return ASYS_RESULT_BAD_PARAM;
	if(!out) return ASYS_RESULT_BAD_PARAM;

#ifdef AGA_HAVE_STAT
	{
		struct stat st;

		if(stat(path, &st) == -1) {
			return aga_error_system_path(__FILE__, "stat", path);
		}

		return asys_file_attribute_select_stat(&st, attr, out);
	}
#else
	if(attr == ASYS_FILE_TYPE) return asys_file_attribute_type(path, out);

	{
		enum asys_result result;

		void* fp;

		if(!(fp = fopen(path, "rb"))) {
			return aga_error_system_path(__FILE__, "fopen", path);
		}

		if((result = asys_file_attribute_select(fp, attr, out))) return result;

		if(fclose(fp) == EOF) return aga_error_system(__FILE__, "fclose");

		return ASYS_RESULT_OK;
	}
#endif
}
 */

enum asys_result asys_path_attribute(
		const char* path, enum asys_file_attribute_type type,
		union asys_file_attribute* attribute) {

#ifdef AGA_DEVBUILD
# ifdef ASYS_WIN32
(void) path;
(void) type;
(void) attribute;

return ASYS_RESULT_NOT_IMPLEMENTED;
# elif defined(ASYS_UNIX)
	struct stat buffer;

	if(stat(path, &buffer) == -1) {
		return asys_result_errno_path(__FILE__, "stat", path);
	}

	return asys_file_attribute_stat(&buffer, type, attribute);
# elif defined(ASYS_STDC)
	(void) path;
	(void) type;
	(void) attribute;

	return ASYS_RESULT_NOT_IMPLEMENTED;
# else
	(void) path;
	(void) type;
	(void) attribute;

	return ASYS_RESULT_NOT_IMPLEMENTED;
# endif
#else
	(void) path;
	(void) type;
	(void) attribute;

	return ASYS_RESULT_NOT_IMPLEMENTED;
#endif
}

enum asys_result asys_path_tail(
		const char* path, void* buffer, asys_size_t count) {

#ifdef AGA_DEVBUILD
	enum asys_result result;

	struct asys_stream stream;

	result = asys_stream_new(&stream, path);
	if(result) return result;

	result = asys_stream_seek(&stream, ASYS_SEEK_END, -(asys_offset_t) count);
	if(result) goto cleanup;

	result = asys_stream_read(&stream, 0, buffer, count);
	if(result) goto cleanup;

	return asys_stream_delete(&stream);

	cleanup: {
		asys_log_result(
				__FILE__, "asys_stream_delete", asys_stream_delete(&stream));

		return result;
	}
#else
	(void) path;
	(void) buffer;
	(void) count;

	return ASYS_RESULT_NOT_IMPLEMENTED;
#endif

}

enum asys_result asys_path_remove(const char* path) {
#ifdef AGA_DEVBUILD
# ifdef ASYS_WIN32
	(void) path;

	return ASYS_RESULT_NOT_IMPLEMENTED;
# elif defined(ASYS_UNIX)
	(void) path;

	return ASYS_RESULT_NOT_IMPLEMENTED;
# elif defined(ASYS_STDC)
	if(remove(path) == -1) {
		return asys_result_errno_path(__FILE__, "remove", path);
	}

	return ASYS_RESULT_OK;
# else
	(void) path;

	return ASYS_RESULT_NOT_IMPLEMENTED;
# endif
#else
	(void) path;

	return ASYS_RESULT_NOT_IMPLEMENTED;
#endif
}

/* TODO: Implement BSD-y `<sys/dir.h>' `direct' interface  */
enum asys_result asys_path_iterate(
		const char* path, aga_directory_callback_t callback,
		asys_bool_t recurse, void* pass, asys_bool_t keep_going) {

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
				result = asys_path_iterate(
						buffer, fn, recurse, pass, keep_going);

				if(result) {
					if(keep_going) {
						asys_log_result_path(
								__FILE__, "asys_path_iterate", buffer,
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
						__FILE__, "asys_path_iterate::<callback>", buffer,
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
	union asys_file_attribute attr;

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

		if((result = asys_path_attribute(buffer, ASYS_FILE_TYPE, &attr))) {
			if(keep_going) {
				asys_log_result_path(
						__FILE__, "asys_path_attribute", buffer, result);

				held_result = result;
				continue;
			}
			else return result;
		}

		if(attr.type == ASYS_FILE_DIRECTORY) {
			if(recurse) {
				result = asys_path_iterate(
						buffer, fn, recurse, pass, keep_going);

				if(result) {
					if(keep_going) {
						asys_log_result_path(
								__FILE__, "asys_path_iterate", buffer,
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
						__FILE__, "asys_path_iterate::<callback>", buffer,
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
	(void) callback;
	(void) recurse;
	(void) pass;
	(void) keep_going;

	return ASYS_RESULT_NOT_IMPLEMENTED;
#endif
}
