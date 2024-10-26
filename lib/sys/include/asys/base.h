/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_BASE_H
#define ASYS_BASE_H

#ifdef _WIN32
# define ASYS_WIN32
# ifdef _WIN64
#  define ASYS_LP32
# endif
#elif defined(unix) || defined(__unix) || defined(__unix__)
# define ASYS_UNIX
# define ASYS_STDC
#else
# define ASYS_STDC
#endif

#ifdef ASYS_LP32
# ifdef __GNUC__
__extension__
# endif
typedef long long asys_native_long_t;
# ifdef __GNUC__
__extension__
# endif
typedef unsigned long long asys_native_ulong_t;
# define ASYS_NATIVE_LONG_FORMAT "%lld"
# define ASYS_NATIVE_ULONG_FORMAT "%llu"
# define ASYS_MAKE_NATIVE_LONG(v) (v##LL)
# define ASYS_MAKE_NATIVE_ULONG(v) (v##ULL)
#else
typedef long asys_native_long_t;
typedef unsigned long asys_native_ulong_t;
# define ASYS_NATIVE_LONG_FORMAT "%ld"
# define ASYS_NATIVE_ULONG_FORMAT "%lu"
# define ASYS_MAKE_NATIVE_LONG(v) (v##L)
# define ASYS_MAKE_NATIVE_ULONG(v) (v##UL)
#endif

asys_native_long_t asys_native_strtol(const char*, char**, int);

#endif
