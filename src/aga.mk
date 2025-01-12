# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>

AGA = src$(SEP)aga$(SEP)
AGAH = include$(SEP)aga$(SEP)

AGAN = src$(SEP)agan$(SEP)
AGANH = include$(SEP)agan$(SEP)

# aga
AGA1 = $(AGA)config.c $(AGA)draw.c $(AGA)midi.c $(AGA)pack.c $(AGA)graph.c
AGA2 = $(AGA)python.c $(AGA)script.c $(AGA)startup.c $(AGA)render.c
AGA3 = $(AGA)sound.c $(AGA)aga.c $(AGA)window.c $(AGA)build.c
# agan
AGA4 = $(AGAN)draw.c $(AGAN)agan.c $(AGAN)object.c $(AGAN)utility.c $(AGAN)io.c
AGA5 = $(AGAN)math.c $(AGAN)editor.c

# TODO: Temporary.
AGA6 = $(ASYS)main.c

# aga
AGAH1 = $(AGAH)config.h $(AGAH)gl.h $(AGAH)script.h $(AGAH)pack.h $(AGAH)draw.h
AGAH2 = $(AGAH)python.h $(AGAH)sound.h $(AGAH)startup.h $(AGAH)render.h
AGAH3 = $(AGAH)window.h $(AGAH)graph.h
# agan
AGAH4 = $(AGANH)agan.h $(AGANH)object.h $(AGANH)draw.h $(AGAH)render.h
AGAH5 = $(AGANH)utility.h $(AGANH)io.h
# TODO: `sys' headers.

AGA_SRC = $(AGA1) $(AGA2) $(AGA3) $(AGA4) $(AGA5) $(AGA6)
AGA_HDR = $(AGAH1) $(AGAH2) $(AGAH3) $(AGAH4) $(AGAH5)
AGA_OBJ = $(subst .c,$(OBJ),$(AGA_SRC))

AGA_OUT = $(AGA)aga$(EXE)

$(AGA_OBJ): $(ASYS_HDR) $(APRO_HDR)
$(AGA_OBJ): $(PY_HDR) $(WWW_HDR) $(GLM_HDR) $(TIFF_HDR)
$(AGA_OBJ): $(AGA_HDR)

$(AGA_OUT): $(ASYS_OUT) $(APRO_OUT)
$(AGA_OUT): $(PY_OUT) $(WWW_OUT) $(GLM_OUT) $(TIFF_OUT) $(MIL_OUT)
$(AGA_OUT): $(AGA_OBJ)
	$(LINK)

$(AGA)script.c: $(PYGRAM)

$(AGA)window$(OBJ): $(AGA)sys$(SEP)x$(SEP)window.h
$(AGA)window$(OBJ): $(AGA)sys$(SEP)win32$(SEP)window.h

$(AGA)midi$(OBJ): $(AGA)sys$(SEP)win32$(SEP)midi.h

clean_aga:
	$(RM) $(AGA_OBJ) $(AGA_OUT)
