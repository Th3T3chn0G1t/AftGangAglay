# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>

AGA = src$(SEP)aga$(SEP)
AGAH = include$(SEP)aga$(SEP)

AGAN = src$(SEP)agan$(SEP)
AGANH = include$(SEP)agan$(SEP)

# aga
AGA1 = $(AGA)config.c $(AGA)draw.c $(AGA)midi.c $(AGA)pack.c
AGA2 = $(AGA)python.c $(AGA)script.c $(AGA)startup.c
AGA3 = $(AGA)sound.c $(AGA)win32.c $(AGA)aga.c $(AGA)window.c
AGA4 = $(AGA)render.c $(AGA)build.c $(AGA)graph.c
# agan
AGA5 = $(AGAN)draw.c $(AGAN)agan.c $(AGAN)object.c $(AGAN)utility.c
AGA6 = $(AGAN)math.c $(AGAN)editor.c $(AGAN)io.c

# aga
AGAH1 = $(AGAH)config.h $(AGAH)gl.h $(AGAH)script.h
AGAH2 = $(AGAH)python.h $(AGAH)sound.h $(AGAH)startup.h $(AGAH)render.h
AGAH3 = $(AGAH)win32.h $(AGAH)window.h $(AGAH)pack.h $(AGAH)draw.h
AGAH4 = $(AGAH)graph.h
# agan
AGAH5 = $(AGANH)agan.h $(AGANH)object.h $(AGANH)draw.h $(AGAH)render.h
AGAH6 = $(AGANH)utility.h $(AGANH)io.h
# TODO: `sys' headers.

AGA_SRC = $(AGA1) $(AGA2) $(AGA3) $(AGA4) $(AGA5) $(AGA6)
AGA_HDR = $(AGAH1) $(AGAH2) $(AGAH3) $(AGAH4) $(AGAH5) $(AGAH6)
AGA_OBJ = $(subst .c,$(OBJ),$(AGA_SRC))

AGA_OUT = $(AGA)aga$(EXE)

$(AGA_OBJ): $(ASYS_HDR) $(APRO_HDR)
$(AGA_OBJ): $(PY_HDR) $(WWW_HDR) $(DEV_HDR)
$(AGA_OBJ): $(AGA_HDR)

$(AGA_OUT): $(ASYS_OUT) $(APRO_OUT)
$(AGA_OUT): $(PY_OUT) $(WWW_OUT) $(DEV_LIBS)
$(AGA_OUT): $(AGA_OBJ)
	$(GL_CCLD)

$(AGA)script.c: $(PYGRAM)

$(AGA)window$(OBJ): $(AGA)xwindow.h
$(AGA)window$(OBJ): $(AGA)win32window.h

$(AGA)midi$(OBJ): $(AGA)win32midi.h

clean_aga:
	$(RM) $(AGA_OBJ) $(AGA_OUT)
