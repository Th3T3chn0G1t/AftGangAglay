/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_SOUND_H
#define AGA_SOUND_H

#include <asys/base.h>
#include <asys/result.h>

struct aga_resource;

struct aga_sound_stream {
	struct aga_resource* resource;

	asys_bool_t loop;
	asys_bool_t done;

	asys_bool_t did_finish;
	asys_size_t last_seek;

	asys_size_t offset;
};

struct aga_sound_device {
	int fd;

	asys_uchar_t* buffer;
	asys_uchar_t* scratch;

	asys_size_t size;

	struct aga_sound_stream* streams;
	asys_size_t count;
};

enum asys_result aga_sound_device_new(struct aga_sound_device*, asys_size_t);
enum asys_result aga_sound_device_delete(struct aga_sound_device*);

enum asys_result aga_sound_device_update(struct aga_sound_device*);

/* Start a new sound stream into the device */
enum asys_result aga_sound_play(
		struct aga_sound_device*, struct aga_resource*, asys_bool_t,
		asys_size_t*);

#endif
