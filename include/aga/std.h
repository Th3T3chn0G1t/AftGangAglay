/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_STD_H
#define AGA_STD_H

#include <aga/environment.h>

#ifdef _WIN32
# ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
# endif
# ifndef _CRT_NONSTDC_NO_WARNINGS
#  define _CRT_NONSTDC_NO_WARNINGS
# endif
#endif

#ifndef _MSC_VER
# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 2
# endif
#endif

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4668) /* Symbol not defined as macro. */
#endif

#include <asys/stdc.h>

#ifdef AGA_WANT_UNIX
# undef AGA_WANT_UNIX
# ifdef AGA_HAVE_UNISTD
#  include <unistd.h>
# endif
# ifdef AGA_HAVE_GETOPT
#  include <getopt.h>
# endif
# ifdef AGA_HAVE_SYS_STAT
#  include <sys/stat.h>
# endif
# ifdef AGA_HAVE_SYS_TYPES
#  include <sys/types.h>
# endif
# ifdef AGA_HAVE_FCNTL
#  include <fcntl.h>
# endif
# ifdef AGA_HAVE_SYS_WAIT
#  include <sys/wait.h>
# endif
# ifdef AGA_HAVE_DIRENT
#  include <dirent.h>
# endif
# ifdef _WIN32
#  include <io.h>
#  include <direct.h>
# endif
#endif

#ifdef _MSC_VER
# pragma warning(pop)
#endif

#ifdef _WIN32
# undef _CRT_SECURE_NO_WARNINGS
# undef _CRT_NONSTDC_NO_WARNINGS
#endif

#ifndef _MSC_VER
# undef _POSIX_C_SOURCE
#endif

#endif
