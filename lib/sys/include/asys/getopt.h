/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+asys@pm.me>
 */

#ifndef ASYS_GETOPT_H
#define ASYS_GETOPT_H

extern int opterr;
extern int optind;
extern int optopt;
extern char* optarg;

int getopt(int, char**, char*);

/*
 * TODO: Implement MS-DOS style argparse for Windows platforms to replace
 * 		 `getopt'. See here for era-appropriate impl.:
 * 		 https://github.com/microsoft/MS-DOS/v4.0/src/CMD/FC/FC.C:288
 */

#endif
