/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/file.h>
#include <asys/system.h>
#include <asys/error.h>
#include <asys/stream.h>
#include <asys/log.h>
#include <asys/string.h>

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

/*
 * TODO: See here: https://winasm.tripod.com/Clib.html (esp. Note #5) and
 * 		 Clarify what we can/can't do with modern code, and whether we can
 * 		 Switch to older impls. by detecting compilation env./Win32 version.
 */
/*
 * TODO: Does this need to exist in general? We only seem to use it for file
 * 		 Type checks where we could just ask the user to specify file vs.
 * 		 Directory inputs.
 */
enum asys_result asys_path_attribute(
		const char* path, enum asys_file_attribute_type type,
		union asys_file_attribute* attribute) {

#ifdef AGA_DEVBUILD
# ifdef ASYS_WIN32
	/*
	 * TODO: DOS `_dos_getfileattr' compat function seems to be a Borland-ism
	 * 		 Whch no longer exists and has very little documentation.
	 */
	enum asys_result result;

	struct asys_stream stream;

	if(type == ASYS_FILE_TYPE) {
		DWORD attributes;

		/*
		 * Not era-accurate call (`_getfileattr').
		 * Did the historical version return `-1' aswell?
		 */
		if((attributes = GetFileAttributes(path)) == (DWORD) -1) {
			result = ASYS_RESULT_ERROR;
			asys_log_result_path(__FILE__, "GetFileAttributes", path, result);
			return result;
		}

		/* TODO: Does this constant work everywhere today? */
		if(attributes & _A_SUBDIR) attribute->type = ASYS_FILE_DIRECTORY;
		else attribute->type = ASYS_FILE_REGULAR;

		return ASYS_RESULT_OK;
	}

	result = asys_stream_new(&stream, path);
	if(result) return result;

	result = asys_stream_attribute(&stream, type, attribute);
	if(result) goto cleanup;

	result = asys_stream_delete(&stream);
	if(result) return result;

	return ASYS_RESULT_OK;

	cleanup: {
		asys_log_result(
				__FILE__, "asys_stream_delete", asys_stream_delete(&stream));

		return result;
	}
# elif defined(ASYS_UNIX)
	struct stat buffer;

	/* TODO: Is `lseek' or `stat' more efficient for getting file length. */
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
	enum asys_result result;

	/*
	 * TODO: Unclear as to whether Windows treats this as a POSIX compat
	 * 		 Function or a DOS compat function.
	 */
	if(_unlink(path) == -1) {
		/*
		 * TODO: Ensure all Windows call errors log like this for parity with
		 * 		 *nix-y/stdc EH.
		 */
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "_unlink", result);
		return result;
	}

	return ASYS_RESULT_OK;
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

#ifdef ASYS_WIN32
	/*
	 * TODO: DOS' C interface shipped `_dos_' functions for these which no
	 * 		 Longer appear to exist:
	 * 		 		https://github.com/microsoft/MS-DOS/blob/main/
	 * 		 				/v4.0/src/TOOLS/BLD/INC/DOS.H
	 */

	static asys_fixed_buffer_t buffer = { 0 };

	enum asys_result result;
	enum asys_result held_result = ASYS_RESULT_OK;

	/* TODO: `_dos_' variants used `find_t'. */
	struct _finddata_t data;

	asys_native_long_t find;

	result = asys_string_format(&buffer, 0, "%s\\*", path);
	if(result) return result;

	/*
	 * TODO: Should this class of DOS compat function use `_doserrno' or
	 * 		 `dosexterr'?
	 */
	if((find = _findfirst(buffer, &data)) == -1) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "_findfirst", result);
		return result;
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

		result = asys_string_format(&buffer, 0, "%s/%s", path, data.name);
		if(result) {
			if(keep_going) {
				held_result = result;
				continue;
			}
			else return result;
		}

		if(data.attrib & _A_SUBDIR) {
			if(recurse) {
				result = asys_path_iterate(
						buffer, callback, recurse, pass, keep_going);

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
		else if((result = callback(buffer, pass))) {
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

	return held_result;
#elif defined(ASYS_UNIX)
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
