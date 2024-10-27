/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_SYSTEM_H
#define ASYS_SYSTEM_H

#include <asys/base.h>

#ifdef ASYS_WIN32
# include <windows.h>
#endif

#ifdef ASYS_UNIX
# include <unistd.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <getopt.h>
# include <dirent.h>
#endif

#ifdef ASYS_STDC
# include <stdio.h>
# include <stdarg.h>
# include <limits.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <ctype.h>
# include <errno.h>
# include <signal.h>
# ifdef AGA_WANT_MATH
#  include <math.h>
#  ifndef M_PI /* TODO: Remove reliance on this */
#   define M_PI (3.14159265358979323846)
#  endif
# endif
#endif

#include <asys/varargs.h>

#endif
