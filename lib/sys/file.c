/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#include <asys/file.h>
#include <asys/system.h>
#include <asys/error.h>
#include <asys/stream.h>
#include <asys/log.h>

/*
 * TODO: Old stdc-y stat emulation -- reuse me.
static enum asys_result aga_file_attribute_type(
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

enum asys_result aga_file_attribute_length(void* fp, asys_size_t* size) {
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

static enum asys_result aga_file_attribute_select(
		void* fp, enum aga_file_attribute_type attr, union aga_file_attribute* out) {

	switch(attr) {
		default: return ASYS_RESULT_BAD_PARAM;

		case AGA_FILE_MODIFIED: return ASYS_RESULT_NOT_IMPLEMENTED;

		case AGA_FILE_LENGTH: return aga_file_attribute_length(fp, &out->length);

 * If it's a file handle, it's a regular file.
		case AGA_FILE_TYPE: {
			out->type = AGA_REGULAR;
			break;
		}
	}

	return ASYS_RESULT_OK;
}
#endif

enum asys_result aga_file_attribute_path(
		const char* path, enum aga_file_attribute_type attr,
		union aga_file_attribute* out) {

	if(!path) return ASYS_RESULT_BAD_PARAM;
	if(!out) return ASYS_RESULT_BAD_PARAM;

#ifdef AGA_HAVE_STAT
	{
		struct stat st;

		if(stat(path, &st) == -1) {
			return aga_error_system_path(__FILE__, "stat", path);
		}

		return aga_file_attribute_select_stat(&st, attr, out);
	}
#else
	if(attr == AGA_FILE_TYPE) return aga_file_attribute_type(path, out);

	{
		enum asys_result result;

		void* fp;

		if(!(fp = fopen(path, "rb"))) {
			return aga_error_system_path(__FILE__, "fopen", path);
		}

		if((result = aga_file_attribute_select(fp, attr, out))) return result;

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
		return asys_error_errno_path(__FILE__, "stat", path);
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
		return asys_error_errno_path(__FILE__, "remove", path);
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
