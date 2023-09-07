/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_CORE_H
#define AGA_CORE_H

#include <agamath.h>

#include <afeirsa/afeirsa.h>
#include <afeirsa/afgl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

struct aga_vertex {
	float pos[3];
	float col[4];
	float uv[2];
	float norm[3];
};

struct aga_cam {
	union aga_vec3 pos;
	float yaw;
	float pitch;
	float dist;
};

struct aga_ctx {
	struct af_ctx af_ctx;
	int win;

	struct af_vert vert;

	struct aga_cam cam;

	struct aga_settings {
		/* Input */
		float sensitivity;
		float zoom_speed;
		float min_zoom, max_zoom;

		/* Display */
		float fov;
		unsigned width, height;
	} settings;
};

enum af_err aga_init(struct aga_ctx* ctx, int* argcp, char** argvp);
enum af_err aga_kill(struct aga_ctx* ctx);

enum af_err aga_setcam(struct aga_ctx* ctx);

/*
 * NOTE: The use of `chk' is somewhat inconsistent in that the base `AF_'
 * 		 Macros bubble up soft errors whereas the `aga_*_chk' family of
 * 		 Functions are fatal. Just something to keep in mind when error
 * 		 Handling. We may want to add a soft-error form of the errno handling
 * 		 At some point, returning `AF_ERR_ERRNO' and having the user do a
 * 		 `perror' or something.
 */
void aga_af_chk(const char* proc, enum af_err e);
void aga_errno_chk(const char* proc);
void aga_fatal(const char* fmt, ...);

void aga_boundf(float* f, float min, float max);

#endif
