/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_GRAPH_H
#define AGA_GRAPH_H

#include <aga/window.h>

#include <apro.h>

struct asys_main_data;

struct aga_graph {
	struct aga_window window;

	asys_size_t segments;
	asys_size_t max;

	asys_size_t period;
	asys_size_t inter;

	apro_unit_t* running;

	apro_unit_t* histories;
	float* heights;
};

enum asys_result aga_graph_new(
		struct aga_graph*, struct aga_window_device*, struct asys_main_data*);

enum asys_result aga_graph_delete(struct aga_graph*, struct aga_window_device*);

enum asys_result aga_graph_update(struct aga_graph*, struct aga_window_device*);

enum asys_result aga_graph_plot(
		struct aga_graph*, unsigned, unsigned, enum apro_section);

#endif
