/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef ASYS_SYSTEM_H
#define ASYS_SYSTEM_H

#include <asys/base.h>

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
#endif

#include <asys/varargs.h>
#include <asys/detail.h>

#endif
