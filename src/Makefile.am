VER=3
lib_LTLIBRARIES = libido3-0.1.la

ido_built_public_sources =		\
	idotypebuiltins.h

stamp_files =				\
	idotypebuiltins.h		\
	idotypebuiltins.c

sources_h =				\
	idocalendarmenuitem.h		\
	idoentrymenuitem.h		\
	idomessagedialog.h		\
	idorange.h			\
	idoscalemenuitem.h		\
	idoswitchmenuitem.h		\
	idotimeline.h			\
	libido.h 

EXTRA_DIST =				\
	ido.list			\
	idotypebuiltins.h.template	\
	idotypebuiltins.c.template


idotypebuiltins.h: stamp-idotypebuiltins.h

stamp-idotypebuiltins.h: $(sources_h)
	( cd $(srcdir) && $(GLIB_MKENUMS) --template idotypebuiltins.h.template \
		$(sources_h) ) >> xgen-gtbh \
	&& (cmp -s xgen-gtbh idotypebuiltins.h || cp xgen-gtbh idotypebuiltins.h ) \
	&& rm -f xgen-gtbh && echo timestamp > $(@F)

idotypebuiltins.c: stamp-idotypebuiltins.h
	( cd $(srcdir) && $(GLIB_MKENUMS) --template idotypebuiltins.c.template \
		$(sources_h) ) > xgen-gtbc \
	&& cp xgen-gtbc idotypebuiltins.c && rm -f xgen-gtbc

INCLUDES =				\
	-I$(srcdir)			\
	-I$(top_srcdir)			\
	-DG_LOG_DOMAIN=\"IDO\"		\
	-DPREFIX=\"$(prefix)"\"		\
	-DLIBDIR=\"$(libdir)"\"		\
	-DG_DISABLE_DEPRECATED		\
	-DGDK_PIXBUF_DISABLE_DEPRECATED	\
	-DGDK_DISABLE_DEPRECATED

AM_CPPFLAGS =				\
	$(GCC_FLAGS)			\
	$(GTK_CFLAGS)			\
	$(MAINTAINER_CFLAGS)    \
	-Wall -Werror -Wextra -Wno-unused-parameter

AM_CFLAGS =                             \
	$(COVERAGE_CFLAGS)

libido_0_1_la_SOURCES =			\
	idotypebuiltins.c		\
	idocalendarmenuitem.c		\
	idoentrymenuitem.c		\
	idomessagedialog.c		\
	idorange.c			\
	idoscalemenuitem.c		\
	idoswitchmenuitem.c		\
	idotimeline.c                   

libido3_0_1_la_SOURCES = $(libido_0_1_la_SOURCES)

libidoincludedir=$(includedir)/libido$(VER)-0.1/libido

libidoinclude_HEADERS =			\
	idocalendarmenuitem.h		\
	idoentrymenuitem.h		\
	idomessagedialog.h		\
	idorange.h			\
	idoscalemenuitem.h		\
	idoswitchmenuitem.h		\
	idotimeline.h			\
	libido.h

libido_0_1_la_LIBADD = $(GTK_LIBS) $(LIBM)
libido_0_1_la_LDFLAGS =                 \
	$(GTK_LT_LDFLAGS)               \
	$(COVERAGE_LDFLAGS)
libido3_0_1_la_LIBADD = $(libido_0_1_la_LIBADD)
libido3_0_1_la_LDFLAGS =                \
	$(libido_0_1_la_LDFLAGS)        \
	$(COVERAGE_LDFLAGS)

DISTCLEANFILES =			\
	stamp-idotypebuiltins.h		\
	idotypebuiltins.h		\
	idotypebuiltins.c

