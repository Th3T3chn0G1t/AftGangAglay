# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>

LIBTIFF_ROOT = vendor/libtiff

MKG3STATES = $(LIBTIFF_ROOT)/libtiff/mkg3states$(EXE)

LIBTIFF_GENERATED = $(LIBTIFF_ROOT)/libtiff/tif_fax3sm.c
LIBTIFF_GENERATED_OBJECT = $(LIBTIFF_ROOT)/libtiff/tif_fax3sm.o

LIBTIFF_SOURCES = tif_aux.c tif_close.c tif_codec.c tif_compress.c tif_dir.c tif_dirinfo.c tif_dirread.c \
					tif_dirwrite.c tif_dumpmode.c tif_error.c tif_fax3.c tif_getimage.c tif_jpeg.c \
					tif_flush.c tif_lzw.c tif_next.c tif_open.c tif_packbits.c tif_predict.c tif_print.c tif_read.c \
					tif_swab.c tif_strip.c tif_thunder.c tif_tile.c tif_version.c tif_warning.c tif_write.c tif_zip.c \
					tif_unix.c
LIBTIFF_SOURCES := $(addprefix $(LIBTIFF_ROOT)/libtiff/,$(LIBTIFF_SOURCES))
LIBTIFF_OBJECTS = $(LIBTIFF_SOURCES:.c=.o) $(LIBTIFF_GENERATED_OBJECT)

LIBTIFF = vendor/libtiff.a
LIBTIFF_IFLAGS = -isystem $(LIBTIFF_ROOT)/libtiff

LIBTIFF_CFLAGS = -std=c89 -ansi -D_SVID_SOURCE -w -Ivendor/support
LIBTIFF_CFLAGS += -Wno-incompatible-function-pointer-types

ifndef WINDOWS
	LIBTIFF_CFLAGS += -Dunix -DHAVE_MMAP
endif

ifdef DEBUG
	LIBTIFF_CFLAGS += -g -D_DEBUG
else
	LIBTIFF_CFLAGS += -DNDEBUG -O
endif

$(MKG3STATES): CFLAGS = $(LIBTIFF_CFLAGS)
$(MKG3STATES): LDFLAGS =
$(MKG3STATES): LDLIBS =

$(LIBTIFF): CFLAGS = $(LIBTIFF_CFLAGS)
$(LIBTIFF): $(LIBTIFF_OBJECTS)

$(LIBTIFF_GENERATED): $(MKG3STATES)
	$(CROSS_TOOL) $(MKG3STATES) -c const $@

all: $(LIBTIFF)

clean: clean_libtiff
.PHONY: clean_libtiff
clean_libtiff:
	$(call PATHREM,$(MKG3STATES))
	$(call PATHREM,$(LIBTIFF_OBJECTS))
	$(call PATHREM,$(LIBTIFF_GENERATED))
	$(call PATHREM,$(LIBTIFF))
