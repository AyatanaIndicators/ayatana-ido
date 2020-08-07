VER=3

noinst_PROGRAMS =               \
        menus

menus_SOURCES =                 \
        menus.c

menus_CPPFLAGS =                \
        -I$(top_srcdir)         \
        -I$(top_srcdir)/src     \
        -I$(top_builddir)/src   \
        $(GCC_FLAGS)            \
        $(GTK_CFLAGS)           \
        $(MAINTAINER_CFLAGS)

menus_LDADD = $(top_builddir)/src/libayatana-ido$(VER)-0.4.la $(GTK_LIBS)

DISTCLEANFILES = Makefile.in
