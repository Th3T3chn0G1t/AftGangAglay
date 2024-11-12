/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_SYSTEM_H
#define ASYS_SYSTEM_H

#include <asys/base.h>

/*
 * TODO: When did these come about, and is it safe to use with older versions
 * 		 Of Visual C?
 */
#ifdef ASYS_VISUALC
# pragma warning(push)
/* Identifier is not defined as a preprocessor macro. */
# pragma warning(disable: 4668)
#endif

#ifdef ASYS_WIN32
# include <windows.h>
# include <commdlg.h>
# include <dos.h>
# include <io.h> /* TODO: Did this exist? */
/*
 * TODO: This appears to be a modern compat header. Can we examine it for help
 * 		 In using era-accurate alternatives?
 */
# include <windowsx.h>
#endif

#ifdef ASYS_UNIX
# define _POSIX_C_SOURCE 2 /* TODO: We shouldn't rely on this -- remove. */
# include <unistd.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <getopt.h>
# include <dirent.h>
#endif

#if defined(ASYS_STDC) || defined(ASYS_FORCE_STD_INCLUDE) /* TODO: Temp!*/
# include <stdio.h>
# include <limits.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <ctype.h>
# include <errno.h>
# include <signal.h>
#endif

#include <asys/varargs.h>

#ifdef ASYS_VISUALC
# pragma warning(pop)
#endif

#include <asys/detail.h>

#endif
