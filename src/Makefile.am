sources_h =				\
	idoentrymenuitem.h		\
	idoscalemenuitem.h

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
	idoentrymenuitem.c		\
	idoscalemenuitem.c

libidoincludedir=$(includedir)/libido-0.1/libido

libidoinclude_HEADERS =			\
	idoentrymenuitem.h		\
	idoscalemenuitem.h

libido_0_1_la_LIBADD = $(GTK_LIBS)
libido_0_1_la_LDFLAGS = $(GTK_LT_LDFLAGS)

idoheadersdir = $(includedir)/ido-0.1/ido
