/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_SCRIPT_H
#define AGA_SCRIPT_H

#include <asys/base.h>
#include <asys/result.h>

#include <apro.h>

struct aga_resource;
struct aga_resource_pack;

struct aga_keymap;
struct aga_pointer;
struct aga_settings;
struct aga_sound_device;
struct aga_window_device;
struct aga_window;
struct aga_resource_pack;
struct aga_buttons;

struct aga_script_userdata {
	struct aga_keymap* keymap;
	struct aga_pointer* pointer;
	struct aga_settings* opts;
	struct aga_sound_device* sound_device;
	asys_bool_t* die;
	struct aga_window_device* window_device;
	struct aga_window* window;
	struct aga_resource_pack* resource_pack;
	struct aga_buttons* buttons;
	apro_unit_t* dt;
};

struct aga_script_class {
	void* class;

	void* create_attr;
	void* update_attr;
	void* close_attr;
};

struct aga_script_instance {
	struct aga_script_class* class;
	void* object;

	void* create_call;
	void* update_call;
	void* close_call;
};

struct aga_script_engine {
	struct py* py;
	struct py_env* env; /* TODO: This is temporary. */

	void* global;
	void* agan;
};

enum aga_script_instance_method {
	AGA_SCRIPT_CREATE,
	AGA_SCRIPT_UPDATE,
	AGA_SCRIPT_CLOSE
};

enum asys_result aga_script_engine_new(
		struct aga_script_engine*, const char*, struct aga_resource_pack*,
		const char*, void*);

enum asys_result aga_script_engine_delete(struct aga_script_engine*);

/* TODO: Generalised object storage/lookup abstraction. */
enum asys_result aga_script_engine_lookup(
		struct aga_script_engine*, struct aga_script_class*, const char*);

void aga_script_engine_trace(void);

void* aga_script_pointer_new(void*);
void* aga_script_pointer_get(void*);

enum asys_result aga_script_instance_new(
		struct aga_script_class*, struct aga_script_instance*);

enum asys_result aga_script_instance_delete(struct aga_script_instance*);

enum asys_result aga_script_instance_call(
		struct aga_script_engine*, struct aga_script_instance*,
		enum aga_script_instance_method);

#endif
