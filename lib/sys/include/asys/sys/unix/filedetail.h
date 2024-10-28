/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_UNIX_FILEDETAIL_H
#define ASYS_UNIX_FILEDETAIL_H

#include <asys/filedata.h>

struct stat;

enum asys_result asys_file_attribute_stat(
		struct stat*, enum asys_file_attribute_type,
		union asys_file_attribute*);

#endif
