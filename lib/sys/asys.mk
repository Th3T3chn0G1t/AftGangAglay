# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>

ASYS = lib$(SEP)sys$(SEP)
ASYSI = $(ASYS)include$(SEP)
ASYSH = $(ASYSI)asys$(SEP)

ASYS_SRC = $(ASYS)asys.c
ASYS_HDR = $(ASYSH)base.h
ASYS_OBJ = $(subst .c,$(OBJ),$(ASYS_SRC))

ASYS_OUT = lib$(SEP)$(LIB)asys$(A)

$(ASYS_OBJ): $(ASYS_HDR)

$(ASYS_OUT): $(ASYS_OBJ)
	$(AR)

clean_asys:
	$(RM) $(ASYS_OBJ) $(ASYS_OUT)
