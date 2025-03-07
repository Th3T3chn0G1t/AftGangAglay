/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_PYTHON_H
#define AGA_PYTHON_H

#include <asys/base.h>

#include <python/state.h>
#include <python/errors.h>
#include <asys/result.h>
#include <python/ceval.h>
#include <python/traceback.h>
#include <python/compile.h>
#include <python/import.h>
#include <python/parsetok.h>
#include <python/pgen.h>
#include <python/evalops.h>

#include <python/module/builtin.h>
#include <python/module/math.h>

#include <python/object.h>
#include <python/object/float.h>
#include <python/object/string.h>
#include <python/object/module.h>
#include <python/object/int.h>
#include <python/object/list.h>
#include <python/object/tuple.h>
#include <python/object/dict.h>
#include <python/object/class.h>
#include <python/object/method.h>

#endif
