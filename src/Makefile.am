ido_built_public_sources =		\
	idotypebuiltins.h

stamp_files =				\
	idotypebuiltins.h		\
	idotypebuiltins.c

sources_h =				\
	idoentrymenuitem.h		\
	idomessagedialog.h		\
	idoscalemenuitem.h		\
	idotimeline.h			\
	libido.h

EXTRA_DIST =				\
	ido.list

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
	-DGDK_DISABLE_DEPRECATED	\
	-DGTK_DISABLE_DEPRECATED

AM_CPPFLAGS =				\
	$(GCC_FLAGS)			\
	$(GTK_CFLAGS)			\
	$(MAINTAINER_CFLAGS)

lib_LTLIBRARIES = libido-0.1.la

libido_0_1_la_SOURCES =			\
	idotypebuiltins.c		\
	idoentrymenuitem.c		\
	idomessagedialog.c		\
	idoscalemenuitem.c		\
	idotimeline.c

libidoincludedir=$(includedir)/libido-0.1/libido

libidoinclude_HEADERS =			\
	idoentrymenuitem.h		\
	idoscalemenuitem.h		\
	libido.h

libido_0_1_la_LIBADD = $(GTK_LIBS)
libido_0_1_la_LDFLAGS = $(GTK_LT_LDFLAGS)

idoheadersdir = $(includedir)/ido-0.1/ido
